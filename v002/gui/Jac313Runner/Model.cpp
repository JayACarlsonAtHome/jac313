#include "Model.h"
#include <jText.h>
#include <parser/line.hpp>
#include <sstream>
#include <parser/section.hpp>

namespace Upp {

// ---------------------------------------------------------------- options

void RunOptions::Jsonize(JsonIO& io)
{
	io("ctest", ctest)("smoke", smoke)("bench", bench)("verify_lite", verifyLite)("verify", verify)
	  ("compiler", compiler)("build_type", buildType)("hardened", hardened)("contracts", contracts)("front_end", frontEnd)("size", size)
	  ("sqlite", sqlite)("pg_local", pgLocal)("pg_remote", pgRemote)
	  ("jtext", jtext)("binary", binary)("json", json)("html", html)
	  ("max_runs", maxRuns)("max_records", maxRecords)("max_persist_rows", maxRows)("readback", readback)("pg_cleanup", pgCleanup)
	  ("connections_file", connFile);   // the server details live in that file, not here
}

String SettingsPath() { return GetExeDirFile("Jac313Runner.json"); }

static String OneOf(const String& v, const char *a, const char *b, const char *c = NULL)
{
	if(v == a || v == b || (c && v == c))
		return v;
	return a;
}

bool LoadSettings(RunOptions& o)
{
	RunOptions s;
	if(!FileExists(SettingsPath()) || !LoadFromJsonFile(s, SettingsPath()))
		return false;
	// a hand-edited file with an unknown word falls back to the default choice
	s.compiler  = OneOf(s.compiler, "gcc", "clang", "both");
	s.buildType = OneOf(s.buildType, "Debug", "Release");
	s.frontEnd  = OneOf(s.frontEnd, "textual", "modules");
	s.size      = OneOf(s.size, "smoke", "full");
	if(s.contracts != "observe" && s.contracts != "quick_enforce" && s.contracts != "ignore")
		s.contracts = "enforce";
	s.readback  = OneOf(s.readback, "sample", "all");
	s.pgCleanup = OneOf(s.pgCleanup, "keep-last", "delete");
	s.maxRuns = max(s.maxRuns, 0);
	s.maxRecords = LimitMaxRecords(s.maxRecords);
	s.maxRows = max(s.maxRows, 0);
	LimitPersistToRecords(s);
	o = s;
	return true;
}

bool SaveSettings(const RunOptions& o)
{
	return StoreAsJsonFile(o, SettingsPath(), true);
}

String VersionDir()
{
	String d = GetExeFolder();
	for(int guard = 0; guard < 32 && d.GetCount() > 1; guard++) {
		if(GetFileName(d) == kVersion && FileExists(AppendFileName(d, "CMakeLists.txt")))
			return d;
		String up = GetFileFolder(d);
		if(up == d || up.IsEmpty())
			break;
		d = up;
	}
	return Null;
}

// ---------------------------------------------------------------- command

String QuoteArg(const String& a)
{
	if(a.IsEmpty())
		return "''";
	for(char c : a)
		if(!(IsAlNum(c) || strchr("_./,=:+-@%", c))) {
			String q = "'";
			for(char k : a) {
				if(k == '\'')
					q << "'\\''";
				else
					q.Cat(k);
			}
			q << "'";
			return q;
		}
	return a;
}

String RunPlan::Display() const
{
	String s = QuoteArg(exe);
	for(const String& a : args)
		s << ' ' << QuoteArg(a);
	return s;
}

String PersistList(const RunOptions& o)
{
	Vector<String> p;
	if(o.sqlite)   p.Add("sql");
	if(o.pgLocal)  p.Add("pg-local");
	if(o.pgRemote) p.Add("pg-remote");
	if(o.jtext)    p.Add("jtext");
	if(o.binary)   p.Add("binary");
	if(o.json)     p.Add("json");
	if(o.html)     p.Add("html");
	return Join(p, ",");
}

// Highest cxx_std_NN named in the version's CMake files -> "C++NN".
String VersionStandard(const String& vdir)
{
	int best = 0;
	for(const char *f : { "Setup/CMakeLists.txt", "Store/CMakeLists.txt", "tools/CMakeLists.txt" }) {
		String s = LoadFile(AppendFileName(vdir, f));
		for(int at = s.Find("cxx_std_"); at >= 0; at = s.Find("cxx_std_", at + 8)) {
			int n = 0;
			for(int i = at + 8; i < s.GetCount() && IsDigit(s[i]); i++)
				n = n * 10 + (s[i] - '0');
			best = max(best, n);
		}
	}
	return best ? "C++" + AsString(best) : String();
}

String RemoteConninfo(const RunOptions& o)
{
	String h = TrimBoth(o.pgHost);
	if(h.IsEmpty())
		return Null;
	auto word = [](const String& s) {   // libpq quoting: spaces/quotes need '...'
		String t = TrimBoth(s);
		if(t.Find(' ') < 0 && t.Find('\'') < 0 && t.Find('\\') < 0)
			return t;
		String q = "'";
		for(char c : t) {
			if(c == '\'' || c == '\\')
				q << '\\';
			q << c;
		}
		return q + "'";
	};
	String c = "host=" + word(h);
	if(TrimBoth(o.pgPort).GetCount())
		c << " port=" << word(o.pgPort);
	if(TrimBoth(o.pgDb).GetCount())
		c << " dbname=" << word(o.pgDb);
	if(TrimBoth(o.pgUser).GetCount())
		c << " user=" << word(o.pgUser);
	return c << " connect_timeout=10";
}

String LocalConninfo(const Vector<ConnRow>& rows)
{
	RunOptions o;   // only its pg* fields are used, through RemoteConninfo's quoting
	o.pgHost = "localhost";
	o.pgDb = "jac313";
	o.pgUser = "jac313";
	for(const ConnRow& r : rows)
		if(r.backend == "pg-local") {
			if(TrimBoth(r.host).GetCount())
				o.pgHost = r.host;
			o.pgPort = r.port;
			if(TrimBoth(r.dbname).GetCount())
				o.pgDb = r.dbname;
			if(TrimBoth(r.user).GetCount())
				o.pgUser = r.user;
		}
	return RemoteConninfo(o);
}

String FindDatasetTool(const String& vdir)
{
	// any build dir (build-gcc15, build-gcc16, build-clang, -hardened, bench, ...):
	// the compiler preset names its folder, so the list is not fixed; the newest build wins
	String best;
	Time   bestTime = Null;
	for(FindFile ff(AppendFileName(vdir, "build-*")); ff; ff.Next()) {
		if(!ff.IsFolder())
			continue;
		String exe = AppendFileName(ff.GetPath(), "Store/tests/matrix/jac313_store_dataset");
		if(!FileExists(exe))
			continue;
		Time t = FileGetTime(exe);
		if(best.IsEmpty() || t > bestTime) {
			best = exe;
			bestTime = t;
		}
	}
	return best.IsEmpty() ? String(Null) : best;
}

String DefaultConnectionsPath(const String& vdir)
{
	String e = GetEnv("JAC313_CONNECTIONS");
	if(e.GetCount())
		return e;
	// <beside the repository>/SQL_Secrets: vdir is <repository>/vNNN
	return AppendFileName(AppendFileName(GetFileFolder(GetFileFolder(vdir)), "SQL_Secrets"), "jac313_connections.jtext");
}

String EffectiveConnFile(const RunOptions& o, const String& vdir)
{
	String f = TrimBoth(o.connFile);
	return f.GetCount() ? f : DefaultConnectionsPath(vdir);
}

String PassFileFor(const RunOptions& o, const String& vdir, bool remote)
{
	String f = AppendFileName(GetFileFolder(EffectiveConnFile(o, vdir)),
	                          remote ? "jac313_pg_remote.pgpass" : "jac313_pg_local.pgpass");
	return FileExists(f) ? f : String();
}

String WithPassFile(const String& conninfo, const String& passfile)
{
	if(passfile.IsEmpty() || conninfo.IsEmpty() || conninfo.Find("passfile=") >= 0
	   || conninfo.StartsWith("postgresql://") || conninfo.StartsWith("postgres://"))   // a URI takes no key=value
		return conninfo;
	String q = passfile;
	if(q.Find(' ') >= 0 || q.Find('\'') >= 0 || q.Find('\\') >= 0) {
		q = "'";
		for(char c : passfile) {
			if(c == '\'' || c == '\\')
				q << '\\';
			q << c;
		}
		q << "'";
	}
	return conninfo + " passfile=" + q;
}

String EnclosingGitProject(const String& path)
{
	String d = GetFileFolder(NormalizePath(path));
	for(int guard = 0; guard < 64 && d.GetCount() > 1; guard++) {
		if(FileExists(AppendFileName(d, ".git")) || DirectoryExists(AppendFileName(d, ".git")))
			return d;
		String up = GetFileFolder(d);
		if(up == d || up.IsEmpty())
			break;
		d = up;
	}
	return Null;
}

bool LoadConnections(const String& path, Vector<ConnRow>& rows, String& err)
{
	rows.Clear();
	if(!FileExists(path)) {
		err = "not found";
		return false;
	}
	JDoc doc;
	doc.Parse(LoadFile(path), GetFileFolder(path));
	for(const JSection& s : doc.sections) {
		auto col = [&](const char *name) {
			for(int i = 0; i < s.cols.GetCount(); i++)
				if(ToLower(s.cols[i]) == name)
					return i;
			return -1;
		};
		int cb = col("backend"), ch = col("host");
		if(cb < 0 || ch < 0)
			continue;
		int cp = col("port"), cd = col("dbname"), cu = col("user"), ct = col("connect_timeout"), cn = col("notes");
		int cm = col("name");   // absent in a file written before it: every name ''
		for(const Vector<String>& r : s.rows) {
			auto get = [&](int c) {
				if(c < 0 || c >= r.GetCount() || r[c] == JNull())
					return String();
				return r[c];
			};
			ConnRow cr;
			cr.backend = get(cb);
			cr.host = get(ch);
			cr.port = get(cp);
			cr.dbname = get(cd);
			cr.user = get(cu);
			cr.timeout = get(ct);
			cr.notes = get(cn);
			cr.name = get(cm);
			if(cr.backend.GetCount() && cr.host.GetCount())
				rows.Add(pick(cr));
		}
	}
	if(rows.IsEmpty()) {
		err = "no rows with a backend and a host";
		return false;
	}
	return true;
}

String ConnectionsText(const Vector<ConnRow>& rows)
{
	using namespace ::jac313::jText::v002;
	const Date d = GetSysDate();
	const String date = Format("%04d-%02d-%02d", d.year, d.month, d.day);
	std::ostringstream os;
	os << "//File:    jac313_connections.jtext\n"
	   << "//Date:    " << date.ToStd() << "\n"
	   << "//Purpose: PostgreSQL servers for jac313 test runs (pg-local / pg-remote).\n"
	   << "//         Keep this file OUTSIDE every git project. No passwords here: libpq reads them\n"
	   << "//         jac313_pg_local.pgpass / jac313_pg_remote.pgpass beside it. Jac313Runner writes it; safe to edit by hand.\n"
	   << "//\n";
	JTextWriter w(os);
	w.set_profile(JTextProfile::Full);
	w.set_filename("jac313_connections.jtext");
	w.set_date(date.ToStd());
	w.set_purpose("PostgreSQL connections for jac313 / Jac313Runner");
	w.write_header();
	w.begin_section("connections");
	w.begin_fields();
	static const char *fields[][2] = {
		{ "id/Identity(1,1)/Not Null", "record id" },
		{ "backend/String/Not Null",   "pg-local | pg-remote" },
		{ "host/String/Not Null",      "host name or IP address" },
		{ "port/Number",               "5432 is PostgreSQL's usual port" },
		{ "dbname/String",             "" },
		{ "user/String",               "" },
		{ "connect_timeout/Number",    "seconds" },
		{ "notes/String",              "" },
		{ "name/String",               "the server's name on every report ('' = its host)" },
	};
	for(int i = 0; i < __countof(fields); i++) {
		std::vector<std::string> levels;
		for(const String& l : Split(fields[i][0], '/'))
			levels.push_back(l.ToStd());
		w.append_row(i + 1, levels, fields[i][1], U'/');
	}
	w.begin_data();
	for(int i = 0; i < rows.GetCount(); i++) {
		const ConnRow& r = rows[i];
		JTextEntry e;
		e.number = 1;
		e.fields = { std::to_string(i + 1), r.backend.ToStd(), r.host.ToStd(),
		             (r.port.GetCount() ? r.port : String("5432")).ToStd(), r.dbname.ToStd(), r.user.ToStd(),
		             (r.timeout.GetCount() ? r.timeout : String("10")).ToStd(), r.notes.ToStd(), r.name.ToStd() };   // '' = none, never {NULL}
		w.append_entry(e);
		w.end_record();
	}
	w.finalize();
	return String(os.str());
}

bool SaveConnections(const String& path, const Vector<ConnRow>& rows, String& err)
{
	String dir = GetFileFolder(path);
	if(!DirectoryExists(dir)) {
		if(!RealizeDirectory(dir)) {
			err = "cannot create " + dir;
			return false;
		}
		chmod(dir, 0700);   // a new secrets folder: owner only
	}
	if(!SaveFile(path, ConnectionsText(rows))) {
		err = "cannot write " + path;
		return false;
	}
	chmod(path, 0600);
	return true;
}

// ---------------------------------------------------------------- production run

String ProductionOptions::Spec() const
{
	return "keeper=" + keeper + ",database=" + database + ",keeper_pct=" + AsString(keeperPct)
	     + ",database_pct=" + AsString(databasePct) + ",binary=" + (binary ? "on" : "off");
}

String ProductionParamsFile(const String& vdir)
{
	return AppendFileName(AppendFileName(vdir, "tests"), "production.jtext");
}

bool LoadProduction(const String& file, ProductionOptions& p, String& err)
{
	if(!FileExists(file)) {
		err = "not found: " + file;
		return false;
	}
	JDoc d;
	d.Parse(LoadFile(file), GetFileFolder(file));
	for(const JSection& s : d.sections) {
		if(s.name != "production")
			continue;
		if(s.rows.GetCount() != 1) {
			err = "the production section must have one row";
			return false;
		}
		auto get = [&](const char *col) -> String {
			int i = FindIndex(s.cols, String(col));
			return i >= 0 && i < s.rows[0].GetCount() ? s.rows[0][i] : String();
		};
		ProductionOptions q;
		q.keeper = get("keeper_sink");
		q.database = get("database_sink");
		q.keeperPct = StrInt(get("keeper_pct"));
		q.databasePct = StrInt(get("database_pct"));
		String b = ToLower(get("binary"));
		q.binary = b == "true" || b == "yes" || b == "y" || b == "t" || b == "1";
		if((q.keeper != "jtext" && q.keeper != "html" && q.keeper != "json") ||
		   (q.database != "sql" && q.database != "pg-local" && q.database != "pg-remote") ||
		   IsNull(q.keeperPct) || q.keeperPct < 0 || q.keeperPct > 100 ||
		   IsNull(q.databasePct) || q.databasePct < 0 || q.databasePct > 100) {
			err = "the production row holds a value outside its choices";
			return false;
		}
		p = q;
		return true;
	}
	err = "no production section in " + file;
	return false;
}

bool SaveProduction(const String& file, const ProductionOptions& p, String& err)
{
	using namespace ::jac313::jText::v002;
	// The new row through jText's writer: a one-section document, whose data lines then replace
	// the file's production data lines (replace_section_data keeps every other line as it is).
	std::ostringstream os;
	{
		JTextWriter w(os);
		w.set_profile(JTextProfile::Full);
		w.set_filename("production.jtext");
		w.write_header();
		w.begin_section("production");
		w.begin_fields();
		static const char *fields[] = {
			"id/Integer/Not Null", "keeper_sink/String/Not Null/Not Empty", "database_sink/String/Not Null/Not Empty",
			"keeper_pct/Integer/Not Null", "database_pct/Integer/Not Null", "binary/Bool/Not Null",
		};
		for(int i = 0; i < __countof(fields); i++) {
			std::vector<std::string> levels;
			for(const String& l : Split(fields[i], '/'))
				levels.push_back(l.ToStd());
			w.append_row(i + 1, levels, "", U'/');
		}
		w.begin_data();
		JTextEntry e;   // "|" and "#" like the rest of the file
		e.number = 1;
		e.delimiter = U'#';
		e.level_sep = U'|';
		e.fields = { std::string("1"), p.keeper.ToStd(), p.database.ToStd(), AsString(p.keeperPct).ToStd(),
		             AsString(p.databasePct).ToStd(), std::string(p.binary ? "true" : "false") };
		w.append_entry(e);
		w.end_record();
		w.finalize();
	}
	const std::string doc = os.str();
	const auto parsed = jtext::parse_file_structure(doc);
	if(!parsed || parsed->sections.size() != 1) {
		err = "the jText writer's production row did not parse";
		return false;
	}
	std::string data;
	{
		std::istringstream in(doc);
		std::string line;
		for(std::size_t n = 0; std::getline(in, line); n++)
			if(n >= parsed->sections[0].data_begin && n < parsed->sections[0].data_end)
				data += line + "\n";
	}
	const String text = LoadFile(file);
	const auto out = jtext::replace_section_data(std::string_view(~text, text.GetCount()), "production", data);
	if(!out) {
		const std::string& m = out.error().message;
		err = Format("%s: line %d: %s", file, (int)out.error().line_no, String(m.data(), (int)m.size()));
		return false;
	}
	if(!SaveFile(file, String(out->data(), (int)out->size()))) {
		err = "cannot write " + file;
		return false;
	}
	return true;
}

int LimitMaxRecords(int n)
{
	return n <= 0 ? 0 : max(n, kMinMaxRecords);
}

void LimitPersistToRecords(RunOptions& o)
{
	if(o.maxRecords > 0 && (o.maxRows == 0 || o.maxRows > o.maxRecords))
		o.maxRows = o.maxRecords;
}

Array<RunPlan> BuildPlans(const RunOptions& opts, const String& vdir)
{
	RunOptions o = opts;
	LimitPersistToRecords(o);
	Array<RunPlan> plans;
	Vector<String> compilers;
	if(o.compiler == "both")
		compilers << "gcc" << "clang";
	else
		compilers << (o.compiler == "clang" ? "clang" : "gcc");
	const String& cwd = vdir;
	for(const String& c : compilers) {
		RunPlan& p = plans.Add();
		p.cwd = cwd;
		p.exe = AppendFileName(cwd, "jac313_test_cli");
		p.compiler = c;
		// "both": one summary per compiler so the second run does not overwrite the first
		p.summary = AppendFileName(cwd, compilers.GetCount() > 1 ? "test-summary/runner_last_" + c + ".jtext"
		                                                         : String("test-summary/runner_last.jtext"));
		Vector<String>& a = p.args;
		if(o.ctest)      a << "--ctest";
		if(o.smoke)      a << "--smoke";
		if(o.bench)      a << "--bench";
		if(o.verifyLite) a << "--verify-lite";
		if(o.verify)     a << "--verify";
		a << (c == "clang" ? "--clang" : "--gcc");
		if(o.buildType == "Release") a << "--release";
		String cf = EffectiveConnFile(o, vdir);
		if(FileExists(cf))
			a << "--connections" << cf;
		if(o.pgRemote && RemoteConninfo(o).GetCount())   // the fields win (they may be unsaved edits)
			a << "--pg-remote" << RemoteConninfo(o);
		if(o.hardened)               a << "--hardened";
		// a mode this compiler lacks (with "both", observe on clang) runs it with enforce instead
		Vector<String> has = Split(c == "clang" ? o.clangContracts : o.gccContracts, ',');
		String mode = (has.IsEmpty() || FindIndex(has, o.contracts) >= 0) ? o.contracts : String("enforce");
		if(mode != "enforce")        a << "--contracts" << mode;
		if(o.frontEnd == "modules")  a << "--modules";
		if(o.size == "full")         a << "--size" << "full";
		String persist = PersistList(o);
		a << "--persist" << (persist.GetCount() ? persist : String("none"));
		if(o.maxRuns > 0)    a << "--max-runs" << AsString(o.maxRuns);
		if(o.maxRecords > 0) a << "--max-records" << AsString(o.maxRecords);
		if(o.maxRows > 0)    a << "--max-persist-rows" << AsString(o.maxRows);
		if(o.bench)          a << "--production" << o.prod.Spec();
		a << "--readback" << (o.readback == "all" ? "all" : "sample");
		a << "--pg-cleanup" << (o.pgCleanup == "delete" ? "delete" : "keep-last");
		a << "--summary" << p.summary;
	}
	return plans;
}

// ---------------------------------------------------------------- summary

bool ResultRow::Failed() const
{
	String s = ToUpper(status);
	return s != "PASS" && s != "OK" && s != "SKIP" && s != "SKIPPED";
}

bool Summary::Load(const String& file, String& err)
{
	path = file;
	if(!FileExists(file)) {
		err = "no summary file " + file;
		return false;
	}
	return Parse(LoadFile(file), err);
}

bool Summary::Parse(const String& text, String& err)
{
	JDoc d;
	d.Parse(text, path.GetCount() ? GetFileFolder(path) : String());
	auto Section = [&](const char *name) -> const JSection * {
		for(const JSection& s : d.sections)
			if(s.name == name)
				return &s;
		return nullptr;
	};
	// a cell by column name: "" when the column or the value is missing, or NULL
	auto Get = [](const JSection& s, int row, const char *col) -> String {
		int c = FindIndex(s.cols, String(col));
		if(c < 0 || c >= s.rows[row].GetCount())
			return String();
		const String& v = s.rows[row][c];
		return v == JNull() ? String() : v;
	};
	const JSection *run = Section("run");
	if(!run || run->rows.IsEmpty()) {
		err = "not a run summary (no run section)" + (path.GetCount() ? ": " + path : String());
		if(d.notes.GetCount())
			err << " - " << d.notes[0];
		return false;
	}
	contract  = Nvl(StrInt(Get(*run, 0, "contract")), 1);
	if(contract > kRunnerContract) {
		err = Format("summary contract %d is newer than this GUI understands (%d): rebuild the GUI "
		             "(jac313_test_cli gui-build)", contract, kRunnerContract);
		return false;
	}
	runId     = Get(*run, 0, "run_id");
	host      = Get(*run, 0, "host");
	os        = Get(*run, 0, "os");
	version   = Get(*run, 0, "version");
	compiler  = Get(*run, 0, "compiler");
	buildType = Get(*run, 0, "build_type");
	contracts = Get(*run, 0, "contracts");
	started   = Get(*run, 0, "started_utc");
	finished  = Get(*run, 0, "finished_utc");
	command   = Get(*run, 0, "command");
	pgRemote  = Get(*run, 0, "pg_remote");
	pass      = Nvl(StrInt(Get(*run, 0, "pass")), 0);
	fail      = Nvl(StrInt(Get(*run, 0, "fail")), 0);
	settingName.Clear();
	settingValue.Clear();
	if(const JSection *set = Section("settings"))
		for(int i = 0; i < set->rows.GetCount(); i++) {
			settingName.Add(Get(*set, i, "name"));
			settingValue.Add(Get(*set, i, "value"));
		}
	rows.Clear();
	if(const JSection *rs = Section("results"))
		for(int i = 0; i < rs->rows.GetCount(); i++) {
			ResultRow& o = rows.Add();
			o.gate       = Get(*rs, i, "gate");
			o.test       = Get(*rs, i, "test");
			o.backend    = Get(*rs, i, "backend");
			o.outputMode = Get(*rs, i, "output_mode");
			o.status     = Get(*rs, i, "status");
			o.ms         = Get(*rs, i, "ms");
			o.rows       = Get(*rs, i, "rows_written");
			o.log        = Get(*rs, i, "log");
			o.error      = Get(*rs, i, "error");
			o.rbMode     = Get(*rs, i, "rb_mode");
			o.rbCount    = Get(*rs, i, "rb_count");
			o.rbChecksum = Get(*rs, i, "rb_checksum");
			o.rbSample   = Get(*rs, i, "rb_sample");
			o.rbChecked  = Get(*rs, i, "rb_checked");
			o.pingMs     = Get(*rs, i, "rb_ping_ms");
			o.writeMs    = Get(*rs, i, "rb_write_ms");
			o.dsBackend  = Get(*rs, i, "dataset_backend");
			o.dsBase     = Get(*rs, i, "dataset_base");
			o.compiler   = compiler;
			o.buildType  = buildType;
			o.version    = version;
			o.pgRemote   = pgRemote;
		}
	return true;
}

String Summary::SettingsText() const
{
	String t;
	for(int i = 0; i < settingName.GetCount(); i++)
		t << settingName[i] << ": " << settingValue[i] << "\n";
	return t;
}

// ---------------------------------------------------------------- Copy for AI

String AiContextPath(const String& vdir)
{
	return AppendFileName(AppendFileName(vdir, "gui"), "ai_context.md");
}

String AiReportPath(const Summary& s)
{
	String p = s.path.GetCount() ? s.path : String("runner_last.jtext");
	return GetFileFolder(p).GetCount() ? AppendFileName(GetFileFolder(p), GetFileTitle(p) + "_ai.md")
	                                   : GetFileTitle(p) + "_ai.md";
}

static String Cell(const String& s)
{
	String r = s.IsEmpty() ? String("-") : s;
	r.Replace("|", "\\|");
	r.Replace("\n", " ");
	return r;
}

static String Fence(const String& text, int maxLines, bool tail)
{
	Vector<String> l = Split(text, '\n', false);
	int from = 0, to = l.GetCount();
	bool cut = l.GetCount() > maxLines;
	if(cut) {
		if(tail) from = to - maxLines;
		else     to = maxLines;
	}
	String out = "```text\n";
	if(cut && tail)
		out << Format("... (%d earlier lines left out)\n", from);
	for(int i = from; i < to; i++)
		out << l[i] << "\n";
	if(cut && !tail)
		out << Format("... (%d more lines left out)\n", l.GetCount() - to);
	return out << "```\n";
}

String BuildAiReport(const Array<Summary>& sums, const String& vdir, const AiExtra& x)
{
	String md;
	md << "# jac313 test results (for an AI reader)\n\n"
	   << "This report was made by Jac313Runner's **Copy for AI** button. It has the project "
	      "background, what was run, every result, the logs of any failures and the data the "
	      "user had open. Please explain it in plain language for someone who may not know "
	      "the project.\n\n";

	md << "## Project context\n\n";
	String ctx = LoadFile(AiContextPath(vdir));
	if(ctx.GetCount()) {
		if(ctx.StartsWith("<!--")) {   // drop the editor's note at the top
			int e = ctx.Find("-->");
			if(e >= 0)
				ctx = TrimLeft(ctx.Mid(e + 3));
		}
		md << TrimRight(ctx) << "\n\n";
	}
	else
		md << "(No " << AiContextPath(vdir) << " found.)\n\n";

	md << "## What was run\n\n";
	md << "| | " ;
	for(const Summary& s : sums)
		md << Cell(s.compiler) << " | ";
	md << "\n|---|";
	for(int i = 0; i < sums.GetCount(); i++)
		md << "---|";
	md << "\n";
	auto meta = [&](const char *name, auto get) {
		md << "| " << name << " | ";
		for(const Summary& s : sums)
			md << Cell(get(s)) << " | ";
		md << "\n";
	};
	meta("Version", [](const Summary& s) { return s.version; });
	meta("Compiler", [](const Summary& s) { return s.compiler; });
	meta("Build type", [](const Summary& s) { return s.buildType; });
	meta("Contracts", [](const Summary& s) { return s.contracts; });
	meta("Machine", [](const Summary& s) { return s.host; });
	meta("OS", [](const Summary& s) { return s.os; });
	meta("Remote PostgreSQL", [](const Summary& s) { return s.pgRemote; });
	meta("Run id", [](const Summary& s) { return s.runId; });
	meta("Started (UTC)", [](const Summary& s) { return s.started; });
	meta("Finished (UTC)", [](const Summary& s) { return s.finished; });
	meta("Passed", [](const Summary& s) { return AsString(s.pass); });
	meta("Failed", [](const Summary& s) { return AsString(s.fail); });
	Index<String> names;   // every setting any summary lists, in the order given
	for(const Summary& s : sums)
		for(const String& n : s.settingName)
			names.FindAdd(n);
	for(const String& n : names) {
		md << "| " << Cell(n) << " | ";
		for(const Summary& s : sums) {
			int k = FindIndex(s.settingName, n);
			md << Cell(k >= 0 ? s.settingValue[k] : String()) << " | ";
		}
		md << "\n";
	}
	for(const Summary& s : sums)
		if(s.command.GetCount())
			md << "\nCommand (" << Cell(s.compiler) << "): `" << s.command << "`\n";
	md << "\n";

	md << "## Results\n\n"
	   << "Columns: **Gate** (ctest / smoke / bench), **Test**, **Backend** (storage type), "
	      "**Out** (console output on/off), **Status**, **ms** (run time), **Rows** (rows saved), "
	      "**Read-back** (mode: count / checksum / sample results, rows compared), "
	      "**Ping ms** and **Write ms** (PostgreSQL only).\n\n";
	bool two = sums.GetCount() > 1;
	md << (two ? "| Compiler " : "") << "| Gate | Test | Backend | Out | Status | ms | Rows | Read-back | Ping ms | Write ms |\n"
	   << (two ? "|---" : "") << "|---|---|---|---|---|---|---|---|---|---|\n";
	Vector<const ResultRow*> failed;
	for(const Summary& s : sums)
		for(const ResultRow& r : s.rows) {
			String rb;
			if(r.rbMode.GetCount())
				rb << r.rbMode << ": " << Cell(r.rbCount) << " / " << Cell(r.rbChecksum) << " / "
				   << Cell(r.rbSample) << ", " << Cell(r.rbChecked) << " rows";
			md << (two ? "| " + Cell(s.compiler) + " " : String()) << "| " << Cell(r.gate) << " | " << Cell(r.test)
			   << " | " << Cell(r.backend) << " | " << Cell(r.outputMode) << " | " << Cell(r.status)
			   << " | " << Cell(r.ms) << " | " << Cell(r.rows) << " | " << Cell(rb) << " | "
			   << Cell(r.pingMs) << " | " << Cell(r.writeMs) << " |\n";
			if(r.Failed())
				failed.Add(&r);
		}
	md << "\n";

	md << "## Failures\n\n";
	if(failed.IsEmpty())
		md << "None: every result passed.\n\n";
	for(int i = 0; i < failed.GetCount() && i < 10; i++) {
		const ResultRow& r = *failed[i];
		md << "### " << Cell(r.test) << " / " << Cell(r.backend) << " (" << Cell(r.compiler) << ")\n\n"
		   << "Status: " << Cell(r.status) << "\n\n";
		if(r.error.GetCount())
			md << "Error: " << r.error << "\n\n";
		if(r.log.GetCount() && FileExists(r.log))
			md << "Last lines of `" << r.log << "`:\n\n" << Fence(LoadFile(r.log), 40, true) << "\n";
	}
	if(failed.GetCount() > 10)
		md << Format("(%d more failures left out.)\n\n", failed.GetCount() - 10);

	if(x.datasetText.GetCount()) {
		md << "## What the user had open\n\n**" << x.title << "**\n\n";
		if(x.validation.GetCount())
			md << "Check line: `" << x.validation << "`\n\n";
		md << Fence(x.datasetText, 80, false) << "\n";
	}

	md << "## Questions worth asking\n\n"
	      "- In plain words, did everything work?\n"
	      "- What do the read-back columns prove about the saved data?\n"
	      "- Why is pg-remote slower than pg-local, and does it matter here?\n"
	      "- If something failed: what is the most likely cause, and what should be checked first?\n";
	return md;
}

String BuildDirFor(const String& compiler, const String& buildType)
{
	bool clang   = ToLower(compiler).StartsWith("clang");
	bool release = ToLower(buildType) == "release";
	// the run's own GCC label ("gcc16" on Fedora 44) names its trees; "gcc15" when it has none
	String gcc = "gcc15";
	if(ToLower(compiler).StartsWith("gcc") && compiler.GetCount() > 3 && IsDigit(compiler[3]))
		gcc = ToLower(compiler);
	if(release)
		return clang ? "build-bench-clang" : "build-bench-" + gcc;
	// "Debug-hardened" -> build-gcc16-hardened (the CLI's --hardened trees)
	String flavor;
	int dash = buildType.Find('-');
	if(dash >= 0)
		flavor = buildType.Mid(dash);
	return (clang ? String("build-clang") : "build-" + gcc) + flavor;
}

bool DatasetCommand(const ResultRow& r, const String& vdir, String& exe, Vector<String>& args)
{
	args.Clear();
	exe.Clear();
	if(r.dsBase.IsEmpty())
		return false;
	exe = AppendFileName(vdir,
	                     BuildDirFor(r.compiler, r.buildType) + "/Store/tests/matrix/jac313_store_dataset");
	args << "--backend" << (r.dsBackend.GetCount() ? r.dsBackend : r.backend)
	     << "--base" << r.dsBase << "--limit" << "1000";
	// read a pg-remote dataset from the server that run used (older summaries: $JAC313_PG_REMOTE,
	// which the window sets from the Remote PG fields)
	if((r.dsBackend.GetCount() ? r.dsBackend : r.backend) == "pg-remote" && r.pgRemote.GetCount())
		args << "--conninfo" << r.pgRemote;
	return true;
}

// ---------------------------------------------------------------- jText

const String& JNull()
{
	static String s = "\xE2\x88\x85";
	return s;
}

int JSection::Width() const
{
	int w = cols.GetCount();
	for(const auto& r : rows)
		w = max(w, r.GetCount());
	return w;
}

String JSection::ColName(int i) const
{
	if(i < cols.GetCount() && cols[i].GetCount())
		return cols[i];
	return AsString(i + 1);
}

int JDoc::MainSection() const
{
	for(int i = 0; i < sections.GetCount(); i++)
		if(sections[i].rows.GetCount())
			return i;
	return sections.GetCount() ? 0 : -1;
}

static String ToUpp(const std::string& s) { return String(s.data(), (int)s.size()); }

// The Unit Separator byte, or its escape as text, is NULL
// {NULL}, or the v0.7 marker U+001F the library reads as it, is the author's "no value":
// shown as JNull(). Every other value is its text (SPEC 2.6.1).
static String CleanValue(const std::string& v)
{
	if(v == jtext::kNullValue || v == "\x1F")
		return JNull();
	return ToUpp(v);
}

// The jText library parses; this turns its sections into named columns and rows.
// A blank line ends a record and a line for field 1 starts one (SPEC 2.6); each line's
// number is the field position. A field with no line is empty.
void JDoc::Parse(const String& text, const String& baseDir)
{
	sections.Clear();
	notes.Clear();
	const auto file = jtext::parse_file_structure(std::string_view(~text, text.GetCount()));
	if(!file) {
		notes.Add(Format("line %d: %s", (int)file.error().line_no, ToUpp(file.error().message)));
		return;
	}
	if(std::string w = jtext::version_warning(jtext::declared_version(*file)); !w.empty())
		notes.Add(ToUpp(w));
	for(const jtext::parsed_section& ps : file->sections) {
		JSection& s = sections.Add();
		s.name = ps.name.empty() ? String("(unnamed)") : ToUpp(ps.name);
		auto AddField = [&](unsigned n, const std::string& name) {
			if(s.cols.GetCount() < (int)n)
				s.cols.SetCount(n);
			s.cols[n - 1] = ToUpp(name);
		};
		for(const jtext::parsed_line& fl : ps.field_lines)
			if(fl.number && fl.values.size())
				AddField(fl.number, fl.values[0]);
		for(const auto& [kind, path] : ps.includes) {   // a Fields include names the columns
			if(ToLower(ToUpp(kind)) != "fields")
				continue;
			String rel = ToUpp(path);
			String p = IsFullPath(rel) ? rel : NormalizePath(AppendFileName(baseDir, rel));
			if(!FileExists(p)) {
				notes.Add("include not found: " + rel + " (columns named 1..N)");
				continue;
			}
			for(String l : Split(LoadFile(p), '\n', false)) {
				l.TrimEnd("\r");
				const auto fl = jtext::parse_line(std::string_view(~l, l.GetCount()));
				if(fl && fl->kind == jtext::line_kind::data && fl->values.size())
					AddField(fl->number, fl->values[0]);
			}
		}
		Vector<String> rec;
		for(const jtext::parsed_line& pl : ps.data_lines) {
			if(pl.number == 0 || (pl.number == 1 && rec.GetCount())) {
				if(rec.GetCount())
					s.rows.Add(pick(rec));
				rec.Clear();
				if(pl.number == 0)
					continue;
			}
			int first = pl.number - 1;
			if(rec.GetCount() < first + (int)pl.values.size())
				rec.SetCount(first + (int)pl.values.size(), String());
			for(size_t k = 0; k < pl.values.size(); k++)
				rec[first + (int)k] = CleanValue(pl.values[k]);
		}
		if(rec.GetCount())
			s.rows.Add(pick(rec));
		for(int i = 0; i < s.cols.GetCount(); i++)
			if(s.cols[i].IsEmpty())
				s.cols[i] = AsString(i + 1);
	}
}

// ---------------------------------------------------------------- reports as HTML

static String HtmlEsc(const String& s)
{
	String r;
	for(char c : s)
		switch(c) {
		case '&': r << "&amp;"; break;
		case '<': r << "&lt;"; break;
		case '>': r << "&gt;"; break;
		case '"': r << "&quot;"; break;
		default:  r.Cat(c);
		}
	return r;
}

static bool MdWordChar(int c) { return IsAlNum(c) || (c & 0x80); }

static String MdInline(const String& s, const Function<String (const String&)>& link)
{
	String r;
	bool strong = false, em = false;
	int n = s.GetCount();
	for(int i = 0; i < n; i++) {
		char c = s[i];
		if(c == '\\' && i + 1 < n && strchr("\\`*_[]()#|-!.<>", s[i + 1])) {
			r << HtmlEsc(String(s[++i], 1));
			continue;
		}
		if(c == '`') {
			int e = s.Find('`', i + 1);
			if(e > i) {
				r << "<code>" << HtmlEsc(s.Mid(i + 1, e - i - 1)) << "</code>";
				i = e;
				continue;
			}
		}
		if(c == '[') {
			int m = s.Find("](", i + 1);
			int e = m > i ? s.Find(')', m + 2) : -1;
			if(e > m) {
				r << "<a href=\"" << HtmlEsc(link(s.Mid(m + 2, e - m - 2))) << "\">"
				  << MdInline(s.Mid(i + 1, m - i - 1), link) << "</a>";
				i = e;
				continue;
			}
		}
		if(c == '*' && i + 1 < n && s[i + 1] == '*') {
			r << (strong ? "</strong>" : "<strong>");
			strong = !strong;
			i++;
			continue;
		}
		if(c == '_') {   // _x_ only at word edges, so snake_case names stay as they are
			bool before = i > 0 && MdWordChar((byte)s[i - 1]);
			bool after = i + 1 < n && MdWordChar((byte)s[i + 1]);
			if(!em && !before && i + 1 < n && s[i + 1] != ' ') { r << "<em>"; em = true; continue; }
			if(em && !after && i > 0 && s[i - 1] != ' ') { r << "</em>"; em = false; continue; }
		}
		r << HtmlEsc(String(c, 1));
	}
	if(em) r << "</em>";
	if(strong) r << "</strong>";
	return r;
}

static Vector<String> MdCells(const String& line)
{
	String t = TrimBoth(line);
	if(t.StartsWith("|")) t.Remove(0);
	if(t.EndsWith("|") && !t.EndsWith("\\|")) t.Trim(t.GetCount() - 1);
	Vector<String> cells;
	String cur;
	bool code = false;
	for(int i = 0; i < t.GetCount(); i++) {
		char c = t[i];
		if(c == '\\' && i + 1 < t.GetCount() && t[i + 1] == '|') { cur << "\\|"; i++; continue; }
		if(c == '`') code = !code;
		if(c == '|' && !code) { cells.Add(TrimBoth(cur)); cur.Clear(); continue; }
		cur.Cat(c);
	}
	cells.Add(TrimBoth(cur));
	return cells;
}

String MarkdownToHtml(const String& md, Function<String (const String&)> link)
{
	Vector<String> lines = Split(md, '\n', false);
	for(String& l : lines)
		l.TrimEnd("\r");
	String title, body;
	String para;
	auto flush = [&] {
		if(para.GetCount())
			body << "<p>" << para << "</p>\n";
		para.Clear();
	};
	for(int i = 0; i < lines.GetCount(); i++) {
		const String& l = lines[i];
		String t = TrimBoth(l);
		if(t.IsEmpty()) { flush(); continue; }
		if(t.StartsWith("<")) { flush(); body << t << "\n"; continue; }   // raw HTML (anchors)
		if(t == "---") { flush(); body << "<hr>\n"; continue; }
		int h = 0;
		while(h < t.GetCount() && h < 6 && t[h] == '#') h++;
		if(h && h < t.GetCount() && t[h] == ' ') {
			flush();
			String text = t.Mid(h + 1);
			if(title.IsEmpty()) title = text;
			body << Format("<h%d>", h) << MdInline(text, link) << Format("</h%d>\n", h);
			continue;
		}
		if(t.StartsWith("|")) {
			flush();
			Vector<String> head = MdCells(t);
			Vector<String> align;
			int j = i + 1;
			if(j < lines.GetCount() && TrimBoth(lines[j]).StartsWith("|")) {
				Vector<String> a = MdCells(lines[j]);
				bool isAlign = true;
				for(const String& x : a)
					for(char c : x)
						if(c != '-' && c != ':') isAlign = false;
				if(isAlign) {
					for(const String& x : a)
						align.Add(x.EndsWith(":") ? (x.StartsWith(":") ? "center" : "right") : "left");
					j++;
				}
			}
			auto cell = [&](const char *tag, const String& v, int k) {
				body << "<" << tag;
				if(k < align.GetCount() && align[k] != "left")
					body << " style=\"text-align:" << align[k] << "\"";
				body << ">" << MdInline(v, link) << "</" << tag << ">";
			};
			body << "<table>\n<thead><tr>";
			for(int k = 0; k < head.GetCount(); k++) cell(align.GetCount() ? "th" : "td", head[k], k);
			body << "</tr></thead>\n<tbody>\n";
			for(; j < lines.GetCount() && TrimBoth(lines[j]).StartsWith("|"); j++) {
				Vector<String> row = MdCells(lines[j]);
				body << "<tr>";
				for(int k = 0; k < row.GetCount(); k++) cell("td", row[k], k);
				body << "</tr>\n";
			}
			body << "</tbody></table>\n";
			i = j - 1;
			continue;
		}
		if(t.StartsWith("- ")) {
			flush();
			body << "<ul>\n";
			int j = i;
			for(; j < lines.GetCount() && TrimBoth(lines[j]).StartsWith("- "); j++)
				body << "<li>" << MdInline(TrimBoth(lines[j]).Mid(2), link) << "</li>\n";
			body << "</ul>\n";
			i = j - 1;
			continue;
		}
		if(para.GetCount()) para << "<br>\n";
		para << MdInline(t, link);
	}
	flush();
	return "<!doctype html>\n<html lang=\"en\">\n<head>\n<meta charset=\"utf-8\">\n"
	       "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
	       "<title>" + HtmlEsc(title.GetCount() ? title : String("Report")) + "</title>\n<style>\n"
	       ":root{color-scheme:light dark;--bg:#fff;--fg:#1d1d1f;--mut:#6e6e73;--line:#d2d2d7;--head:#f2f2f5;--link:#0b63c5}\n"
	       "@media (prefers-color-scheme:dark){:root{--bg:#1c1c1e;--fg:#f2f2f7;--mut:#a1a1a6;--line:#3a3a3c;--head:#2c2c2e;--link:#6cb4ff}}\n"
	       "body{background:var(--bg);color:var(--fg);font:15px/1.5 system-ui,-apple-system,'Segoe UI',sans-serif;"
	       "max-width:1200px;margin:0 auto;padding:16px 24px}\n"
	       "a{color:var(--link)}em{color:var(--mut)}h1,h2,h3{line-height:1.25}\n"
	       "table{border-collapse:collapse;margin:12px 0;display:block;overflow-x:auto}\n"
	       "th,td{border:1px solid var(--line);padding:4px 10px;white-space:nowrap}th{background:var(--head)}\n"
	       "tbody tr:nth-child(even){background:var(--head)}\n"
	       "code{font:13px ui-monospace,Menlo,Consolas,monospace}hr{border:0;border-top:1px solid var(--line)}\n"
	       "</style>\n</head>\n<body>\n" + body + "</body>\n</html>\n";
}

static void MdFiles(const String& root, const String& rel, Vector<String>& out)
{
	String dir = rel.GetCount() ? AppendFileName(root, rel) : root;
	for(FindFile ff(AppendFileName(dir, "*")); ff; ff.Next()) {
		String r = rel.GetCount() ? rel + "/" + ff.GetName() : ff.GetName();
		if(ff.IsFolder())
			MdFiles(root, r, out);
		else if(ff.IsFile() && ToLower(GetFileExt(r)) == ".md")
			out.Add(r);
	}
}

int ExportReportHtml(const String& srcDir, const String& dstDir, String& err)
{
	Vector<String> pages;
	MdFiles(srcDir, Null, pages);
	if(pages.IsEmpty()) {
		err = "no .md pages in " + srcDir;
		return 0;
	}
	Index<String> known;
	for(const String& p : pages)
		known.Add(p);
	if(DirectoryExists(dstDir))
		DeleteFolderDeep(dstDir);
	for(const String& p : pages) {
		String pdir = GetFileFolder(p);   // "" or "ctest"
		auto link = [&](const String& url) -> String {
			if(url.StartsWith("#") || url.Find("://") >= 0)
				return url;
			int h = url.Find('#');
			String path = h >= 0 ? url.Left(h) : url, frag = h >= 0 ? url.Mid(h) : String();
			if(ToLower(GetFileExt(path)) != ".md")
				return url;
			String abs = NormalizePath(AppendFileName(AppendFileName(srcDir, pdir), path));
			String rel = abs.StartsWith(srcDir + "/") ? abs.Mid(srcDir.GetCount() + 1) : String();
			if(rel.GetCount() && known.Find(rel) >= 0)
				return ForceExt(path, ".html") + frag;   // a converted page: same relative link
			return "file://" + abs + frag;              // outside the report: the original file
		};
		String html = MarkdownToHtml(LoadFile(AppendFileName(srcDir, p)), link);
		String dst = ForceExt(AppendFileName(dstDir, p), ".html");
		if(!RealizeDirectory(GetFileFolder(dst)) || !SaveFile(dst, html)) {
			err = "cannot write " + dst;
			return 0;
		}
	}
	return pages.GetCount();
}

}
