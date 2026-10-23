#include "Model.h"

namespace Upp {

// ---------------------------------------------------------------- options

void RunOptions::Jsonize(JsonIO& io)
{
	io("root", root)("version", version)
	  ("ctest", ctest)("smoke", smoke)("bench", bench)("verify_lite", verifyLite)("verify", verify)
	  ("compiler", compiler)("build_type", buildType)("strict", strict)("hardened", hardened)("front_end", frontEnd)("size", size)
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
	s.readback  = OneOf(s.readback, "sample", "all");
	s.pgCleanup = OneOf(s.pgCleanup, "keep-last", "delete");
	s.maxRuns = max(s.maxRuns, 0);
	s.maxRecords = max(s.maxRecords, 0);
	s.maxRows = max(s.maxRows, 0);
	LimitPersistToRecords(s);
	o = s;
	return true;
}

bool SaveSettings(const RunOptions& o)
{
	return StoreAsJsonFile(o, SettingsPath(), true);
}

String FindRootFromExe()
{
	String d = GetExeFolder();
	for(int guard = 0; guard < 32 && d.GetCount() > 1; guard++) {
		if(FindVersions(d).GetCount())
			return d;
		String up = GetFileFolder(d);
		if(up == d || up.IsEmpty())
			break;
		d = up;
	}
	return Null;
}

String EffectiveRoot(const RunOptions& o, const String& cmdRoot)
{
	if(cmdRoot.GetCount())
		return NormalizePath(cmdRoot);
	if(o.root.GetCount() && DirectoryExists(o.root))
		return o.root;
	return FindRootFromExe();
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

static bool IsVersionName(const String& n)
{
	if(n.GetCount() != 4 || n[0] != 'v')
		return false;
	for(int i = 1; i < 4; i++)
		if(!IsDigit(n[i]))
			return false;
	return true;
}

// Highest cxx_std_NN named in the version's CMake files -> "C++NN".
static String VersionStandard(const String& vdir)
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

Vector<VersionInfo> FindVersions(const String& root)
{
	Vector<VersionInfo> out;
	if(root.IsEmpty())
		return out;
	FindFile ff(AppendFileName(root, "v*"));
	for(; ff; ff.Next()) {
		if(!ff.IsFolder() || !IsVersionName(ff.GetName()))
			continue;
		String vdir = AppendFileName(root, ff.GetName());
		if(!FileExists(AppendFileName(vdir, "CMakeLists.txt")))
			continue;
		VersionInfo& v = out.Add();
		v.name = ff.GetName();
		v.standard = VersionStandard(vdir);
	}
	Sort(out, [](const VersionInfo& a, const VersionInfo& b) { return a.name < b.name; });
	return out;
}

String ResolveVersion(const String& wanted, const String& root)
{
	Vector<VersionInfo> vs = FindVersions(root);
	for(const VersionInfo& v : vs)
		if(v.name == wanted)
			return wanted;
	// nothing chosen yet: the newest version whose bootstrap has run (it has ./jac313_test_cli),
	// so a new user who bootstrapped v001 does not land on a v002 that cannot start
	for(int i = vs.GetCount() - 1; i >= 0; i--)
		if(FileExists(AppendFileName(AppendFileName(root, vs[i].name), "jac313_test_cli")))
			return vs[i].name;
	return vs.GetCount() ? vs.Top().name : wanted;
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

String FindDatasetTool(const String& root, const String& version)
{
	// any build dir (build-gcc15, build-gcc16, build-clang, -strict, -hardened, bench, ...):
	// the compiler preset names its folder, so the list is not fixed; the newest build wins
	String vdir = AppendFileName(root, version);
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

String DefaultConnectionsPath(const String& root)
{
	String e = GetEnv("JAC313_CONNECTIONS");
	if(e.GetCount())
		return e;
	String r = root;
	while(r.GetCount() > 1 && r.EndsWith("/"))
		r.Trim(r.GetCount() - 1);
	return AppendFileName(AppendFileName(GetFileFolder(r), "SQL_Secrets"), "jac313_connections.jtext");
}

String EffectiveConnFile(const RunOptions& o, const String& root)
{
	String f = TrimBoth(o.connFile);
	return f.GetCount() ? f : DefaultConnectionsPath(root);
}

String PassFileFor(const RunOptions& o, const String& root, bool remote)
{
	String f = AppendFileName(GetFileFolder(EffectiveConnFile(o, root)),
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
		for(const Vector<String>& r : s.rows) {
			auto get = [&](int c) {
				if(c < 0 || c >= r.GetCount() || r[c] == JNull() || r[c] == "\\x1F" || r[c] == "\\N" || r[c] == "NULL")
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
	// a field separator that occurs in no value (compact rows split on it)
	char sep = '|';
	for(char c : { '|', '~', '^', '`' }) {
		bool clash = false;
		for(const ConnRow& r : rows)
			for(const String *v : { &r.backend, &r.host, &r.port, &r.dbname, &r.user, &r.timeout, &r.notes })
				if(v->Find(c) >= 0)
					clash = true;
		if(!clash) {
			sep = c;
			break;
		}
	}
	auto val = [](const String& v) { return v.IsEmpty() ? String("\x1F") : v; };   // NULL = Unit Separator
	String t;
	t << "//File:    jac313_connections.jtext\n"
	  << "//Date:    " << Format("%04d-%02d-%02d", GetSysDate().year, GetSysDate().month, GetSysDate().day) << "\n"
	  << "//Purpose: PostgreSQL servers for jac313 test runs (pg-local / pg-remote).\n"
	  << "//         Keep this file OUTSIDE every git project. No passwords here: libpq reads them\n"
	  << "//         jac313_pg_local.pgpass / jac313_pg_remote.pgpass beside it. Jac313Runner writes it; safe to edit by hand.\n"
	  << "//\n"
	  << "=== jText File ===\n"
	  << " 1. #?# jac313_connections\n"
	  << " 3. #?# PostgreSQL connections for jac313 / Jac313Runner\n\n\n"
	  << "=== Section: connections ===\n\n\n"
	  << "=== Fields ===\n"
	  << " 1. #/# id/Identity(1,1)/Not Null     # record id\n"
	  << " 2. #/# backend/String/Not Null       # pg-local | pg-remote\n"
	  << " 3. #/# host/String/Not Null          # host name or IP address\n"
	  << " 4. #/# port/Number                   # 5432 is PostgreSQL's usual port\n"
	  << " 5. #/# dbname/String\n"
	  << " 6. #/# user/String\n"
	  << " 7. #/# connect_timeout/Number        # seconds\n"
	  << " 8. #/# notes/String\n"
	  << "=== End Fields ===\n\n\n"
	  << "=== Data ===\n";
	for(int i = 0; i < rows.GetCount(); i++) {
		const ConnRow& r = rows[i];
		t << "1. #" << sep << "# " << (i + 1) << sep << r.backend << sep << r.host << sep
		  << (r.port.GetCount() ? r.port : String("5432")) << sep << val(r.dbname) << sep << val(r.user) << sep
		  << (r.timeout.GetCount() ? r.timeout : String("10")) << sep << val(r.notes) << "\n\n";
	}
	t << "=== End Data ===\n=== End Section ===\n=== End File ===\n";
	return t;
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

void LimitPersistToRecords(RunOptions& o)
{
	if(o.maxRecords > 0 && (o.maxRows == 0 || o.maxRows > o.maxRecords))
		o.maxRows = o.maxRecords;
}

Array<RunPlan> BuildPlans(const RunOptions& opts, const String& root)
{
	RunOptions o = opts;
	LimitPersistToRecords(o);
	o.version = ResolveVersion(o.version, root);
	Array<RunPlan> plans;
	Vector<String> compilers;
	if(o.compiler == "both")
		compilers << "gcc" << "clang";
	else
		compilers << (o.compiler == "clang" ? "clang" : "gcc");
	String cwd = AppendFileName(root, o.version);
	for(const String& c : compilers) {
		RunPlan& p = plans.Add();
		p.cwd = cwd;
		p.exe = AppendFileName(cwd, "jac313_test_cli");
		p.compiler = c;
		// "both": one summary per compiler so the second run does not overwrite the first
		p.summary = AppendFileName(cwd, compilers.GetCount() > 1 ? "test-summary/runner_last_" + c + ".json"
		                                                         : String("test-summary/runner_last.json"));
		Vector<String>& a = p.args;
		if(o.ctest)      a << "--ctest";
		if(o.smoke)      a << "--smoke";
		if(o.bench)      a << "--bench";
		if(o.verifyLite) a << "--verify-lite";
		if(o.verify)     a << "--verify";
		a << (c == "clang" ? "--clang" : "--gcc");
		if(o.buildType == "Release") a << "--release";
		String cf = EffectiveConnFile(o, root);
		if(FileExists(cf))
			a << "--connections" << cf;
		if(o.pgRemote && RemoteConninfo(o).GetCount())   // the fields win (they may be unsaved edits)
			a << "--pg-remote" << RemoteConninfo(o);
		if(o.strict)                 a << "--strict";
		if(o.hardened)               a << "--hardened";
		if(o.frontEnd == "modules")  a << "--modules";
		if(o.size == "full")         a << "--size" << "full";
		String persist = PersistList(o);
		a << "--persist" << (persist.GetCount() ? persist : String("none"));
		if(o.maxRuns > 0)    a << "--max-runs" << AsString(o.maxRuns);
		if(o.maxRecords > 0) a << "--max-records" << AsString(o.maxRecords);
		if(o.maxRows > 0)    a << "--max-persist-rows" << AsString(o.maxRows);
		a << "--readback" << (o.readback == "all" ? "all" : "sample");
		a << "--pg-cleanup" << (o.pgCleanup == "delete" ? "delete" : "keep-last");
		a << "--summary-json" << p.summary;
	}
	return plans;
}

// ---------------------------------------------------------------- summary

static bool Missing(const Value& v) { return IsError(v) || IsNull(v) || IsVoid(v); }

static String Str(const Value& v)
{
	if(Missing(v))
		return Null;
	if(IsNumber(v)) {
		double d = v;
		if(d == floor(d) && fabs(d) < 9e15)
			return AsString((int64)d);
		return AsString(d);
	}
	if(v.Is<bool>())
		return (bool)v ? "true" : "false";
	return AsString(v);
}

static int Int(const Value& v)
{
	if(IsNumber(v))
		return (int)(double)v;
	return ScanInt(Str(v));
}

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
	Value v = ParseJSON(text);
	if(IsError(v) || !v.Is<ValueMap>()) {
		err = "not a JSON object" + (path.GetCount() ? ": " + path : String());
		return false;
	}
	contract  = IsNull(v["contract"]) ? 1 : Int(v["contract"]);
	if(contract > kRunnerContract) {
		err = Format("summary contract %d is newer than this GUI understands (%d): rebuild the GUI "
		             "(jac313_test_cli gui-build)", contract, kRunnerContract);
		return false;
	}
	runId     = Str(v["run_id"]);
	host      = Str(v["host"]);
	version   = Str(v["version"]);
	compiler  = Str(v["compiler"]);
	buildType = Str(v["build_type"]);
	started   = Str(v["started_utc"]);
	finished  = Str(v["finished_utc"]);
	command   = Str(v["command"]);
	pgRemote  = Str(v["pg_remote"]);
	pass = Int(v["totals"]["pass"]);
	fail = Int(v["totals"]["fail"]);
	rows.Clear();
	Value rs = v["results"];
	for(int i = 0; !Missing(rs) && i < rs.GetCount(); i++) {
		const Value& r = rs[i];
		ResultRow& o = rows.Add();
		o.gate       = Str(r["gate"]);
		o.test       = Str(r["test"]);
		o.backend    = Str(r["backend"]);
		o.outputMode = Str(r["output_mode"]);
		o.status     = Str(r["status"]);
		o.ms         = Str(r["ms"]);
		o.rows       = Str(r["rows_written"]);
		o.log        = Str(r["log"]);
		o.error      = Str(r["error"]);
		const Value& rb = r["readback"];
		if(!Missing(rb)) {
			o.rbMode     = Str(rb["mode"]);
			o.rbCount    = Str(rb["count"]);
			o.rbChecksum = Str(rb["checksum"]);
			o.rbSample   = Str(rb["sample"]);
			o.rbChecked  = Str(rb["checked"]);
			if(o.rbChecked.IsEmpty())
				o.rbChecked = Str(rb["rows_checked"]);
			o.pingMs     = Str(rb["ping_ms"]);
			o.writeMs    = Str(rb["write_ms"]);
		}
		const Value& ds = r["dataset"];
		if(!Missing(ds)) {
			o.dsBackend = Str(ds["backend"]);
			o.dsBase    = Str(ds["base"]);
		}
		const Value& dc = r["dataset_cmd"];
		for(int k = 0; !Missing(dc) && k < dc.GetCount(); k++)
			o.datasetCmd.Add(Str(dc[k]));
		o.compiler  = compiler;
		o.buildType = buildType;
		o.version   = version;
		o.pgRemote  = pgRemote;
	}
	return true;
}

// ---------------------------------------------------------------- Copy for AI

String AiContextPath(const String& root)
{
	return AppendFileName(AppendFileName(root, "gui"), "ai_context.md");
}

String AiReportPath(const Summary& s)
{
	String p = s.path.GetCount() ? s.path : String("runner_last.json");
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

String BuildAiReport(const Array<Summary>& sums, const String& root, const AiExtra& x)
{
	String md;
	md << "# jac313 test results (for an AI reader)\n\n"
	   << "This report was made by Jac313Runner's **Copy for AI** button. It has the project "
	      "background, what was run, every result, the logs of any failures and the data the "
	      "user had open. Please explain it in plain language for someone who may not know "
	      "the project.\n\n";

	md << "## Project context\n\n";
	String ctx = LoadFile(AiContextPath(root));
	if(ctx.GetCount()) {
		if(ctx.StartsWith("<!--")) {   // drop the editor's note at the top
			int e = ctx.Find("-->");
			if(e >= 0)
				ctx = TrimLeft(ctx.Mid(e + 3));
		}
		md << TrimRight(ctx) << "\n\n";
	}
	else
		md << "(No " << AiContextPath(root) << " found.)\n\n";

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
	meta("Machine", [](const Summary& s) { return s.host; });
	meta("Remote PostgreSQL", [](const Summary& s) { return s.pgRemote; });
	meta("Run id", [](const Summary& s) { return s.runId; });
	meta("Started (UTC)", [](const Summary& s) { return s.started; });
	meta("Finished (UTC)", [](const Summary& s) { return s.finished; });
	meta("Passed", [](const Summary& s) { return AsString(s.pass); });
	meta("Failed", [](const Summary& s) { return AsString(s.fail); });
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
	// "Debug-strict-hardened" -> build-gcc16-strict-hardened (the CLI's --strict / --hardened trees)
	String flavor;
	int dash = buildType.Find('-');
	if(dash >= 0)
		flavor = buildType.Mid(dash);
	return (clang ? String("build-clang") : "build-" + gcc) + flavor;
}

bool DatasetCommand(const ResultRow& r, const String& root, String& exe, Vector<String>& args)
{
	args.Clear();
	exe.Clear();
	if(r.datasetCmd.GetCount()) {
		exe = r.datasetCmd[0];
		for(int i = 1; i < r.datasetCmd.GetCount(); i++)
			args.Add(r.datasetCmd[i]);
		return true;
	}
	if(r.dsBase.IsEmpty())
		return false;
	String ver = ResolveVersion(r.version, root);
	exe = AppendFileName(AppendFileName(root, ver),
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

// "  3. #?# value" -> n=3, fmt="?", value="value". False for anything else.
static bool NumberedLine(const String& line, int& n, String& fmt, String& value)
{
	const char *s = line;
	while(*s == ' ' || *s == '\t') s++;
	if(!IsDigit(*s))
		return false;
	n = 0;
	while(IsDigit(*s)) n = n * 10 + (*s++ - '0');
	if(*s++ != '.')
		return false;
	while(*s == ' ' || *s == '\t') s++;
	if(*s == '<' && s[1] == '<' && s[2] == '<') {   // heredoc value: "<<< TAG"
		fmt = "<<<";
		value = TrimBoth(String(s + 3));
		return n > 0;
	}
	if(*s++ != '#')
		return false;
	const char *f = s;
	while(*s && *s != '#' && *s != ' ' && s - f < 4) s++;
	if(*s != '#' || s == f)
		return false;
	fmt = String(f, s);
	s++;
	if(*s == ' ')
		s++;
	value = s;
	return n > 0;
}

static String CleanValue(const String& v)
{
	if(v == "\x1F" || v == "\\x1F" || v == "\\x1f")
		return JNull();
	return v;
}

void JDoc::Parse(const String& text, const String& baseDir)
{
	sections.Clear();
	notes.Clear();
	enum { NONE, FIELDS, DATA } state = NONE;
	JSection *sec = NULL;
	Vector<String> rec;
	bool recUsed = false;
	String heredocTag, heredocText;
	int heredocCol = -1;
	bool inHeredoc = false;

	auto Section = [&]() -> JSection& {
		if(!sec) {
			sec = &sections.Add();
			sec->name = "(unnamed)";
		}
		return *sec;
	};
	auto Flush = [&]() {
		if(recUsed && sec)
			sec->rows.Add(pick(rec));
		rec.Clear();
		recUsed = false;
	};
	auto SetCol = [&](int col, const String& v) {
		if(col < 0)
			return;
		if(rec.GetCount() <= col)
			rec.SetCount(col + 1, JNull());
		rec[col] = v;
		recUsed = true;
	};
	auto AddField = [&](int n, const String& value) {
		JSection& s = Section();
		int slash = value.Find('/');
		String name = TrimBoth(slash >= 0 ? value.Left(slash) : value);
		int hash = name.Find(" #");
		if(hash >= 0)
			name = TrimBoth(name.Left(hash));
		if(s.cols.GetCount() < n)
			s.cols.SetCount(n);
		s.cols[n - 1] = name;
	};
	auto FieldsFromInclude = [&](const String& rel) {
		String p = rel;
		if(!IsFullPath(p))
			p = NormalizePath(AppendFileName(baseDir, p));
		if(!FileExists(p)) {
			notes.Add("include not found: " + rel + " (columns named 1..N)");
			return;
		}
		Vector<String> lines = Split(LoadFile(p), '\n', false);
		for(String l : lines) {
			l.TrimEnd("\r");
			int n; String fmt, v;
			if(NumberedLine(l, n, fmt, v) && fmt == "/")
				AddField(n, v);
		}
	};

	Vector<String> lines = Split(text, '\n', false);
	for(int li = 0; li < lines.GetCount(); li++) {
		String line = lines[li];
		line.TrimEnd("\r");
		if(inHeredoc) {
			if(TrimBoth(line) == heredocTag) {
				inHeredoc = false;
				if(heredocCol >= 0)
					SetCol(heredocCol, heredocText);
				heredocCol = -1;
			}
			else {
				if(heredocText.GetCount())
					heredocText << "\n";
				heredocText << line;
			}
			continue;
		}
		String t = TrimBoth(line);
		if(t == "-- EOF --") {
			Flush();
			break;
		}
		if(t.StartsWith("===") && t.EndsWith("===") && t.GetCount() >= 6) {
			String b = TrimBoth(t.Mid(3, t.GetCount() - 6));
			if(b.StartsWith("<#include#>")) {
				String inc = TrimBoth(b.Mid(11));
				int colon = inc.Find(':');
				String kind = colon >= 0 ? TrimBoth(inc.Left(colon)) : String();
				String rel = colon >= 0 ? TrimBoth(inc.Mid(colon + 1)) : inc;
				if(state == FIELDS || ToLower(kind) == "fields")
					FieldsFromInclude(rel);
				continue;
			}
			Flush();
			if(b.StartsWith("Section:")) {
				sec = &sections.Add();
				sec->name = TrimBoth(b.Mid(8));
				state = NONE;
			}
			else if(b == "Fields") {
				Section();
				state = FIELDS;
			}
			else if(b == "Data") {
				Section();
				state = DATA;
			}
			else
				state = NONE;   // End Fields / End Data / Template: / jText File / End Section ...
			continue;
		}
		if(state == DATA && t.IsEmpty()) {
			Flush();
			continue;
		}
		if(t.IsEmpty() || t.StartsWith("//") || t.StartsWith("#"))
			continue;
		int n; String fmt, value;
		if(!NumberedLine(line, n, fmt, value)) {
			if(state == DATA || state == FIELDS)
				notes.Add(Format("line %d not understood: %s", li + 1, t.Left(60)));
			continue;
		}
		if(fmt == "<<<") {
			inHeredoc = true;
			heredocTag = value;
			heredocText.Clear();
			heredocCol = state == DATA ? n - 1 : -1;
			continue;
		}
		if(state == FIELDS) {
			if(fmt == "/")
				AddField(n, value);
		}
		else if(state == DATA) {
			if(fmt == "|") {
				Vector<String> parts = Split(value, '|', false);
				for(int k = 0; k < parts.GetCount(); k++)
					SetCol(n - 1 + k, CleanValue(parts[k]));
			}
			else
				SetCol(n - 1, CleanValue(value));
		}
		// numbered lines outside Fields/Data are file metadata
	}
	Flush();
	if(inHeredoc)
		notes.Add("unterminated block " + heredocTag);
	for(JSection& s : sections)
		for(int i = 0; i < s.cols.GetCount(); i++)
			if(s.cols[i].IsEmpty())
				s.cols[i] = AsString(i + 1);
}

}
