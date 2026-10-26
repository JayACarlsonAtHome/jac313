// Jac313Runner — run the jac313 test gates and look at what they persisted.
//
// A presenter, like the jac314 GUIs: all real work happens in the engine tool
// jac313_test_cli (and jac313_store_dataset for the viewer). This window builds
// the command line, runs it as a child process, streams its output, then reads
// the summary JSON it wrote.
//
//   Screen 1  Options + Start      (settings saved to Jac313Runner.json beside the program)
//   Screen 2  Running              (progress, pass/fail, elapsed, live output, Stop)
//   Screen 3  Results              (result rows, filters, jText viewer of a row's dataset)
//
// Command line:
//   Jac313Runner [--root R] [--summary FILE] [--jtext FILE]
//     --root R        project root (the jac315 dir); default: saved root, else found
//                     by walking up from the executable
//     --summary FILE  open straight on Screen 3 with this summary JSON
//     --jtext FILE    (with --summary) show this jText file in the viewer
//   Headless (no window, no display needed):
//     Jac313Runner --dump-command [--root R]
//     Jac313Runner --dump-jtext FILE
//     Jac313Runner --dump-summary FILE [--root R]
//   Layout check (needs a display; writes PNGs of the three screens, then exits):
//     Jac313Runner --snap DIR --summary FILE [--jtext FILE]
#include <CtrlLib/CtrlLib.h>
#include <plugin/png/png.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Model.h"

using namespace Upp;

static String gCmdRoot;   // --root
static String gCmdVersion;   // --version vNNN: the version to show first (bootstrap passes its own)

// ======================================================================= headless

static int DumpCommand()
{
	RunOptions o;
	LoadSettings(o);
	String root = EffectiveRoot(o, gCmdRoot);
	if(root.IsEmpty()) {
		Cerr() << "Jac313Runner: no project root (pass --root)\n";
		return 1;
	}
	for(const RunPlan& p : BuildPlans(o, root))
		Cout() << p.Display() << "\n";
	return 0;
}

static String Row(const Vector<String>& v)
{
	return Join(v, "\t");
}

static int DumpJText(const String& file)
{
	if(!FileExists(file)) {
		Cerr() << "Jac313Runner: no file " << file << "\n";
		return 1;
	}
	JDoc d;
	d.Parse(LoadFile(file), GetFileFolder(NormalizePath(file)));
	int si = d.MainSection();
	if(si < 0) {
		Cout() << "columns=0 rows=0\n";
		return 0;
	}
	const JSection& s = d.sections[si];
	int w = s.Width();
	Cout() << "columns=" << w << " rows=" << s.rows.GetCount() << "\n";
	Vector<String> names;
	for(int i = 0; i < w; i++)
		names.Add(s.ColName(i));
	Cout() << Row(names) << "\n";
	for(int r = 0; r < min(3, s.rows.GetCount()); r++) {
		Vector<String> v(s.rows[r], 1);
		v.SetCount(w, JNull());
		Cout() << Row(v) << "\n";
	}
	Cout() << "# section=" << s.name << " sections=" << d.sections.GetCount() << "\n";
	for(const String& n : d.notes)
		Cout() << "# note: " << n << "\n";
	return 0;
}

static String Dash(const String& s) { return s.GetCount() ? s : String("-"); }

static int DumpSummary(const String& file)
{
	Summary s;
	String err;
	if(!s.Load(file, err)) {
		Cerr() << "Jac313Runner: " << err << "\n";
		return 1;
	}
	RunOptions o;
	LoadSettings(o);
	String root = EffectiveRoot(o, gCmdRoot);
	Cout() << "rows=" << s.rows.GetCount() << "\n";
	Cout() << "totals pass=" << s.pass << " fail=" << s.fail << "\n";
	Cout() << "run_id=" << Dash(s.runId) << " host=" << Dash(s.host) << " version=" << Dash(s.version)
	       << " compiler=" << Dash(s.compiler) << " build_type=" << Dash(s.buildType) << "\n";
	Cout() << "Test\tBackend\tRows\tCount\tChecksum\tSample\tms\tPing ms\tWrite ms\tStatus\n";
	for(int i = 0; i < min(3, s.rows.GetCount()); i++) {
		const ResultRow& r = s.rows[i];
		Cout() << Dash(r.test) << "\t" << Dash(r.backend) << "\t" << Dash(r.rows) << "\t" << Dash(r.rbCount) << "\t"
		       << Dash(r.rbChecksum) << "\t" << Dash(r.rbSample) << "\t" << Dash(r.ms) << "\t" << Dash(r.pingMs) << "\t" << Dash(r.writeMs) << "\t" << Dash(r.status) << "\n";
		String exe;
		Vector<String> args;
		if(DatasetCommand(r, root, exe, args)) {
			String c = QuoteArg(exe);
			for(const String& a : args)
				c << ' ' << QuoteArg(a);
			Cout() << "  dataset: " << c << "\n";
		}
		else
			Cout() << "  dataset: -\n";
	}
	return 0;
}

// --dump-ai SUMMARY [--jtext FILE]: print the "Copy for AI" Markdown for a summary.
static int DumpAi(const String& file, const String& jtext)
{
	Array<Summary> sums;
	String err;
	if(!sums.Add().Load(file, err)) {
		Cerr() << "Jac313Runner: " << err << "\n";
		return 1;
	}
	RunOptions o;
	LoadSettings(o);
	AiExtra x;
	if(jtext.GetCount()) {
		x.title = GetFileName(jtext);
		x.datasetText = LoadFile(jtext);
		x.validation = "(headless: no engine validation)";
	}
	Cout() << BuildAiReport(sums, EffectiveRoot(o, gCmdRoot), x);
	return 0;
}

// Returns true when a headless mode ran (the process then exits without a window).
static bool HeadlessMain()
{
	const Vector<String>& a = CommandLine();
	int mode = 0;
	String file, aiJText;
	for(int i = 0; i < a.GetCount(); i++) {
		if(a[i] == "--root" && i + 1 < a.GetCount())
			gCmdRoot = a[++i];
		else if(a[i] == "--dump-command")
			mode = 1;
		else if(a[i] == "--dump-jtext" && i + 1 < a.GetCount()) {
			mode = 2;
			file = a[++i];
		}
		else if(a[i] == "--dump-summary" && i + 1 < a.GetCount()) {
			mode = 3;
			file = a[++i];
		}
		else if(a[i] == "--dump-ai" && i + 1 < a.GetCount()) {
			mode = 4;
			file = a[++i];
		}
		else if(a[i] == "--jtext" && i + 1 < a.GetCount())
			aiJText = a[++i];
	}
	if(!mode)
		return false;
	int rc = mode == 1 ? DumpCommand() : mode == 2 ? DumpJText(file) : mode == 3 ? DumpSummary(file)
	       : DumpAi(file, aiJText);
	SetExitCode(rc);
	return true;
}

#undef GUI_APP_MAIN_HOOK
#define GUI_APP_MAIN_HOOK if(HeadlessMain()) { UPP::AppExit__(); return UPP::GetExitCode(); }

// ======================================================================= helpers

static String StripAnsi(const String& s)
{
	String r;
	for(int i = 0; i < s.GetCount(); i++) {
		char c = s[i];
		if(c == 0x1b && i + 1 < s.GetCount() && s[i + 1] == '[') {
			i += 2;
			while(i < s.GetCount() && !(s[i] >= '@' && s[i] <= '~'))
				i++;
			continue;
		}
		if(c == '\r') {
			if(i + 1 < s.GetCount() && s[i + 1] == '\n')
				continue;
			c = '\n';
		}
		r.Cat(c);
	}
	return r;
}

// "  [116/172] 006_TS | sqlite | off ... PASS (812 ms)" -> i=116 n=172, true
static bool ParseProgress(const String& line, int& i, int& n)
{
	int p = 0;
	while((p = line.Find('[', p)) >= 0) {
		const char *s = ~line + p + 1;
		if(IsDigit(*s)) {
			int a = 0, b = 0;
			while(IsDigit(*s)) a = a * 10 + (*s++ - '0');
			if(*s == '/' && IsDigit(s[1])) {
				s++;
				while(IsDigit(*s)) b = b * 10 + (*s++ - '0');
				if(*s == ']' && b > 0) {
					i = a;
					n = b;
					return true;
				}
			}
		}
		p++;
	}
	return false;
}

static String Elapsed(int ms)
{
	int t = ms / 1000;
	return Format("%d:%02d:%02d", t / 3600, t / 60 % 60, t % 60);
}

static bool HasWord(const String& line, const char *w)
{
	int len = (int)strlen(w);
	int p = 0;
	while((p = line.Find(w, p)) >= 0) {
		bool l = p == 0 || !IsAlNum(line[p - 1]);
		bool r = p + len >= line.GetCount() || !IsAlNum(line[p + len]);
		if(l && r)
			return true;
		p += len;
	}
	return false;
}

static void KillGroup(LocalProcess& p, int sig)
{
	int pid = p.GetPid();
	if(pid > 0) {
		// the child was started through setsid, so its pid is its process group
		if(kill(-pid, sig) != 0)
			kill(pid, sig);
	}
}

// Read everything the child has written so far (LocalProcess reads 1 KB per call).
static bool Drain(LocalProcess& p, String& out, String* err)
{
	bool alive = true;
	for(int k = 0; k < 8192; k++) {
		String o, e;
		alive = err ? p.Read2(o, e) : p.Read(o);
		out << o;
		if(err)
			*err << e;
		if(o.IsEmpty() && e.IsEmpty())
			break;
	}
	return alive;
}

static const char *SETSID = "/usr/bin/setsid";

static bool StartChild(LocalProcess& p, const String& exe, const Vector<String>& args, const String& cwd, bool split)
{
	if(FileExists(SETSID)) {
		Vector<String> a;
		a << exe;
		a.Append(args);
		return split ? p.Start2(SETSID, a, NULL, cwd) : p.Start(SETSID, a, NULL, cwd);
	}
	return split ? p.Start2(exe, args, NULL, cwd) : p.Start(exe, args, NULL, cwd);
}

// Start a program that outlives this window (a browser): double fork, own session, output dropped.
static bool LaunchDetached(const Vector<String>& argv)
{
	if(argv.IsEmpty())
		return false;
	pid_t pid = fork();
	if(pid < 0)
		return false;
	if(pid == 0) {
		if(fork() != 0)
			_exit(0);
		setsid();
		int nul = open("/dev/null", O_RDWR);
		if(nul >= 0) {
			dup2(nul, 0);
			dup2(nul, 1);
			dup2(nul, 2);
		}
		Vector<char *> a;
		for(const String& s : argv)
			a.Add(const_cast<char *>(~s));
		a.Add(nullptr);
		execvp(a[0], a.begin());
		_exit(127);
	}
	int st = 0;
	waitpid(pid, &st, 0);
	return WIFEXITED(st) && WEXITSTATUS(st) == 0;
}

// The first browser found: Chrome, then Chromium, then the desktop default (xdg-open).
static String FindBrowser()
{
	for(const char *b : { "google-chrome", "google-chrome-stable", "chromium", "chromium-browser", "xdg-open" })
		for(const String& d : Split(Nvl(GetEnv("PATH"), String("/usr/bin")), ':'))
			if(FileExists(AppendFileName(d, b)))
				return AppendFileName(d, b);
	return Null;
}

// ======================================================================= window

// An int spin box that says when the user leaves it (U++ edit fields have no such event).
struct LeaveIntSpin : EditIntNotNullSpin {
	Event<> WhenLeave;
	virtual void LostFocus() { EditIntNotNullSpin::LostFocus(); WhenLeave(); }
};

struct Runner : TopWindow {
	typedef Runner CLASSNAME;

	RunOptions opt;
	ParentCtrl s1, s2, s3;

	// ---- Screen 1
	Label     lRemote, lPgHost, lPgPort, lPgDb, lPgUser, pgTestRes;
	EditString pgHost, pgPort, pgDb, pgUser;
	Button    pgTest, pgLTest;
	Label     lConn, connStatus;
	EditString connFile;
	Button    connBrowse, connLoad, connView, connSave;
	Vector<ConnRow> connRows;   // the file's rows as loaded (other backends are kept on Save)
	Label     lRoot, lVersion, lComp, lType, lFront, lSize, lDb, lFiles, lMaxRuns, lMaxRec, lMax, lDefRuns, lDefRec, lDefRows, lRb, lPg, lCmd, lInfo;
	Label     lFormat;   // the format checks every build has (not an option)
	Label     lCompVer;  // "gcc 16.2.1, clang 22.1.8": what gcc / clang mean on this machine
	String    compVerFor;   // the version dir lCompVer was filled for
	EditString root;
	Button    browse;
	LabelBox  boxVersion, boxGates, boxBuild, boxPersist, boxRows;
	Switch    version, comp, type, front, size, rb, pg;
	Option    gCtest, gSmoke, gBench, gVerifyLite, gVerify;
	Option    bStrict, bHardened;
	Option    pSql, pPgL, pPgR, pJt, pBin, pJson, pHtml;
	EditIntNotNullSpin maxRuns;
	LeaveIntSpin maxRecords, maxRows;   // leaving either one limits Max persisted to Max records
	DocEdit   cmd;
	Button    copyCmd, lastResults, saveDef, start;

	// ---- Screen 2
	Label     runTitle, counts, elapsed;
	ProgressIndicator prog;
	LineEdit  out;
	Button    stop, runBack, runResults, runReport, runChrome;

	// ---- Screen 3
	Label     header, lTest, lBackend, vTitle, validation;
	DropList  fTest, fBackend, sectionList;
	Option    failOnly;
	Splitter  split;
	ArrayCtrl list;
	ParentCtrl viewer;
	Switch    viewMode;
	ArrayCtrl table;
	LineEdit  raw;
	Button    openFile, showLog, copyAi, showAi, report, chrome, resBack, quit;
	String    aiPath;     // the *_ai.md that Copy for AI wrote for the results on screen
	Vector<String> reportPages;   // the report's .md pages while the viewer shows them (sectionList = pages)

	// ---- run state
	Array<RunPlan> plans;
	int        planIx = -1;
	LocalProcess proc;
	bool       running = false, stopping = false;
	dword      t0 = 0, stopAt = 0;
	String     lineBuf;
	int        nPass = 0, nFail = 0;
	TimeCallback tick;

	// ---- results state
	Array<Summary> sums;
	Vector<const ResultRow*> all;
	JDoc       doc;
	String     docText;
	LocalProcess dproc;
	bool       dRunning = false;
	String     dOut, dErr, dCmd;

	Runner();
	~Runner();

	String Root() const { return TrimBoth(~root); }
	void   Show(int screen);
	void   ToOptions();
	void   FromOptions();
	void   TestResult(const String& text, bool ok);
	void   UpdateCommand();
	void   CopyCommand();
	void   Browse();
	void   SaveDefault();
	void   Start();
	void   StartPlan();
	void   PlanFinished();
	void   AllFinished();
	void   Feed(const String& s);
	void   Poll();
	void   Stop();
	void   LastResults();
	bool   LoadSummaries(const Vector<String>& files, String& err);
	void   ShowResults();
	void   FillList();
	void   RowSel();
	void   ShowDoc(const String& text, const String& baseDir);
	void   ShowSection();
	void   ViewMode();
	void   OpenFile();
	void   OpenJText(const String& file);
	void   ShowLog();
	void   CopyForAi();
	void   ShowAi();
	String ReportDir();
	void   ShowReport();
	void   ShowReportPage();
	void   OpenReportsInBrowser();
	virtual void Close();
	void   LimitPersistField();
	void   TestRemote() { TestPg(true); }
	void   TestLocal()  { TestPg(false); }
	void   TestPg(bool remote);
	void   LoadConn(bool quiet);
	void   SaveConn();
	void   ViewConn();
	void   BrowseConn();
	void   ConnStatus(const String& text, bool bad);
	String CheckConnFile(const String& path);   // the CLI's jText check line for the file
	void   FillVersions();
	void   ShowCompilerVersions();
	String versionKey;     // the version set FillVersions last built
};

void Runner::LimitPersistField()
{
	ToOptions();   // ToOptions applies LimitPersistToRecords
	if((int)~maxRows != opt.maxRows)
		maxRows <<= opt.maxRows;
	UpdateCommand();
}

// "jac313-002 · 192.168.1.194, 10.10.10.2": this machine's name and its IPv4 addresses, so the
// window says which box it runs on. Loopback, link-local (169.254) and VM/container bridges are left out.
static String MachineLabel()
{
	char name[256] = {};
	String s = gethostname(name, sizeof(name) - 1) == 0 ? String(name) : String("?");
	String ips;
	ifaddrs *list = nullptr;
	if(getifaddrs(&list) == 0) {
		for(ifaddrs *a = list; a; a = a->ifa_next) {
			if(!a->ifa_addr || a->ifa_addr->sa_family != AF_INET || !(a->ifa_flags & IFF_UP) || (a->ifa_flags & IFF_LOOPBACK))
				continue;
			String dev = a->ifa_name;
			if(dev.StartsWith("virbr") || dev.StartsWith("docker") || dev.StartsWith("podman") || dev.StartsWith("cni"))
				continue;
			char buf[INET_ADDRSTRLEN] = {};
			inet_ntop(AF_INET, &((sockaddr_in *)a->ifa_addr)->sin_addr, buf, sizeof(buf));
			String ip = buf;
			if(ip.StartsWith("169.254.") || ips.Find(ip) >= 0)
				continue;
			ips << (ips.GetCount() ? ", " : "") << ip;
		}
		freeifaddrs(list);
	}
	return ips.GetCount() ? s + " · " + ips : s;
}

static void Place(Ctrl& parent, Ctrl& c, int x, int y, int cx, int cy)
{
	parent.Add(c.LeftPosZ(x, cx).TopPosZ(y, cy));
}

Runner::Runner()
{
	Title("Jac313Runner — jac313 test gates — " + MachineLabel());
	Sizeable().Zoomable();
	SetRect(0, 0, Zx(1060), Zy(760));
	SetMinSize(Zsz(760, 560));
	Add(s1.SizePos());
	Add(s2.SizePos());
	Add(s3.SizePos());

	// ================= Screen 1
	int y = 8;
	lRoot.SetText("Project root:");
	Place(s1, lRoot, 8, y + 2, 90, 20);
	s1.Add(root.HSizePosZ(100, 44).TopPosZ(y, 22));
	browse.SetLabel("...");
	s1.Add(browse.RightPosZ(8, 30).TopPosZ(y, 22));
	browse << THISBACK(Browse);
	root.WhenAction = [this] { FillVersions(); UpdateCommand(); };
	y += 32;

	boxVersion.SetLabel("Version");
	s1.Add(boxVersion.HSizePosZ(8, 8).TopPosZ(y, 46));
	// cases come from the vNNN folders under the project root (FillVersions)
	Place(s1, version, 20, y + 18, 600, 22);
	y += 52;

	boxGates.SetLabel("Gates");
	s1.Add(boxGates.HSizePosZ(8, 8).TopPosZ(y, 46));
	gCtest.SetLabel("ctest");
	gSmoke.SetLabel("smoke matrix");
	gBench.SetLabel("bench");
	gVerifyLite.SetLabel("verify-lite");
	gVerify.SetLabel("verify (valgrind)");
	Place(s1, gCtest, 20, y + 18, 80, 22);
	Place(s1, gSmoke, 110, y + 18, 120, 22);
	Place(s1, gBench, 240, y + 18, 80, 22);
	Place(s1, gVerifyLite, 330, y + 18, 110, 22);
	Place(s1, gVerify, 450, y + 18, 160, 22);
	y += 52;

	boxBuild.SetLabel("Build");
	s1.Add(boxBuild.HSizePosZ(8, 8).TopPosZ(y, 124));
	lComp.SetText("Compiler:");
	lType.SetText("Type:");
	lFront.SetText("Front-end:");
	lSize.SetText("Size:");
	// gcc / clang: the newest of each on this machine (the CLI's --gcc / --clang); lCompVer shows
	// which versions those are (jac313_test_cli compiler-versions)
	comp.Add("gcc", "gcc").Add("clang", "clang").Add("both", "both");
	comp.Tip("gcc and clang: the newest of each on this machine (pinned in Setup/compilers.pin). "
	         "The versions are shown to the right.");
	type.Add("Debug", "Debug").Add("Release", "Release");
	front.Add("textual", "textual").Add("modules", "modules");
	size.Add("smoke", "smoke").Add("full", "full");
	Label *bl[] = { &lComp, &lType, &lFront, &lSize };
	Switch *bs[] = { &comp, &type, &front, &size };
	for(int i = 0; i < 4; i++) {
		Place(s1, *bl[i], 20, y + 18 + 26 * i, 90, 22);
		Place(s1, *bs[i], 110, y + 18 + 26 * i, 300, 22);
	}
	Place(s1, lCompVer, 420, y + 18, 400, 22);   // on the Compiler row
	bStrict.SetLabel("strict warnings");
	bHardened.SetLabel("hardened");
	bStrict.Tip("--strict: -Wall -Wextra -Wpedantic and friends (ctest/smoke builds); "
	            "the format checks are on without it");
	bHardened.Tip("--hardened: libstdc++ assertions, zero-init locals, stack/CF protection "
	              "(ctest/smoke builds; not bench or valgrind)");
	Place(s1, bStrict, 420, y + 18 + 26, 130, 22);
	Place(s1, bHardened, 560, y + 18 + 26, 100, 22);
	lFormat.SetText("Always on: -Wformat=2 -Werror=format");
	lFormat.Tip("Every build (any compiler, type or option) treats a printf-style call whose "
	            "arguments do not match its format, such as a 64-bit value given to %d, as an error.");
	Place(s1, lFormat, 420, y + 18 + 78, 360, 22);
	y += 130;

	boxPersist.SetLabel("Persist to");
	s1.Add(boxPersist.HSizePosZ(8, 8).TopPosZ(y, 176));
	lDb.SetText("Databases:");
	lFiles.SetText("Files:");
	pSql.SetLabel("SQLite");
	pPgL.SetLabel("PG local");
	pPgR.SetLabel("PG remote");
	pPgR.Tip("Your own PostgreSQL server on another machine: fill in the Remote PG row below");
	pJt.SetLabel("jText");
	pBin.SetLabel("binary");
	pJson.SetLabel("JSON");
	pHtml.SetLabel("HTML");
	Place(s1, lDb, 20, y + 18, 90, 22);
	Place(s1, pSql, 110, y + 18, 90, 22);
	Place(s1, pPgL, 210, y + 18, 100, 22);
	Place(s1, pPgR, 320, y + 18, 160, 22);
	Place(s1, lFiles, 20, y + 44, 90, 22);
	Place(s1, pJt, 110, y + 44, 90, 22);
	Place(s1, pBin, 210, y + 44, 100, 22);
	Place(s1, pJson, 320, y + 44, 90, 22);
	Place(s1, pHtml, 420, y + 44, 90, 22);
	lMaxRuns.SetText("Max runs:");
	lMaxRec.SetText("Max records in a run:");
	lMax.SetText("Max persisted in a run:");
	auto def = [](int n) {   // "(default 10,000)": thousands separated by commas
		String d = AsString(n), t;
		for(int i = 0; i < d.GetCount(); i++) {
			if(i && (d.GetCount() - i) % 3 == 0)
				t.Cat(',');
			t.Cat(d[i]);
		}
		return "(default " + t + ")";
	};
	lDefRuns.SetText(def(kDefMaxRuns));
	lDefRec.SetText(def(kDefMaxRecords));
	lDefRows.SetText(def(kDefMaxRows));
	maxRuns.Tip("0 = the test's own default");
	maxRecords.Tip("0 = the test's own default");
	maxRows.Tip("0 = no limit (never more than Max records)");
	maxRuns.Min(0);
	maxRecords.Min(0);
	maxRows.Min(0);
	maxRecords.WhenLeave = maxRows.WhenLeave = THISBACK(LimitPersistField);
	// Remote PG: the user's own server (no built-in default). No password field on purpose:
	// libpq reads it from ~/.pgpass or $PGPASSFILE, so it never lands in settings or reports.
	lRemote.SetText("Remote PG:");
	lPgHost.SetText("Host");
	lPgPort.SetText("Port");
	lPgDb.SetText("Database");
	lPgUser.SetText("User");
	pgHost.Tip("Host name or IP address of your PostgreSQL server (needed for PG remote)");
	pgPort.Tip("Empty = 5432");
	pgTest.SetLabel("Test Remote");
	pgTest.Tip("Connect once to the Remote PG server and show the connect time, the round trip (ping) and "
	           "the server version. The password comes from jac313_pg_remote.pgpass beside the connections file.");
	pgTest << THISBACK(TestRemote);
	pgLTest.SetLabel("Test Local");
	pgLTest.Tip("Connect once to this machine's PostgreSQL (the connections file's pg-local row, else "
	            "localhost / jac313 / jac313) and show the connect time, the round trip (ping) and the "
	            "server version. The password comes from jac313_pg_local.pgpass beside the connections file.");
	pgLTest << THISBACK(TestLocal);
	// Connections file: where the server details are kept (jText, outside git)
	lConn.SetText("Connections:");
	connFile.Tip("The PostgreSQL connections file (jText). Keep it OUTSIDE every git project; "
	             "the default is a SQL_Secrets folder beside the project.");
	connBrowse.SetLabel("...");
	connLoad.SetLabel("Load");
	connView.SetLabel("View");
	connView.Tip("Show the connections file in the jText viewer (screen 3)");
	connSave.SetLabel("Save");
	connLoad.Tip("Read the file into the Remote PG fields");
	connSave.Tip("Write the Remote PG fields into the file (creates the folder and file if needed; "
	             "other rows, such as pg-local, are kept)");
	connBrowse << THISBACK(BrowseConn);
	connLoad << [this] { LoadConn(false); };
	connSave << THISBACK(SaveConn);
	connView << THISBACK(ViewConn);
	Place(s1, lConn, 20, y + 70, 90, 22);
	s1.Add(connFile.HSizePosZ(110, 324).TopPosZ(y + 70, 22));
	s1.Add(connBrowse.RightPosZ(284, 30).TopPosZ(y + 70, 22));
	s1.Add(connLoad.RightPosZ(196, 80).TopPosZ(y + 70, 22));
	s1.Add(connView.RightPosZ(108, 80).TopPosZ(y + 70, 22));
	s1.Add(connSave.RightPosZ(20, 80).TopPosZ(y + 70, 22));
	Place(s1, lRemote, 20, y + 96, 90, 22);
	Place(s1, lPgHost, 110, y + 96, 36, 22);
	Place(s1, pgHost, 146, y + 96, 150, 22);
	Place(s1, lPgPort, 304, y + 96, 32, 22);
	Place(s1, pgPort, 336, y + 96, 56, 22);
	Place(s1, lPgDb, 400, y + 96, 62, 22);
	Place(s1, pgDb, 462, y + 96, 100, 22);
	Place(s1, lPgUser, 570, y + 96, 34, 22);
	Place(s1, pgUser, 604, y + 96, 100, 22);
	Place(s1, pgTest, 712, y + 96, 100, 22);
	Place(s1, pgLTest, 818, y + 96, 100, 22);
	s1.Add(pgTestRes.HSizePosZ(926, 16).TopPosZ(y + 96, 22));
	s1.Add(connStatus.HSizePosZ(110, 16).TopPosZ(y + 122, 22));
	Place(s1, lMaxRuns, 20, y + 148, 70, 22);
	Place(s1, maxRuns, 92, y + 148, 70, 22);
	Place(s1, lDefRuns, 166, y + 148, 90, 22);
	Place(s1, lMaxRec, 262, y + 148, 140, 22);
	Place(s1, maxRecords, 402, y + 148, 90, 22);
	Place(s1, lDefRec, 496, y + 148, 110, 22);
	Place(s1, lMax, 612, y + 148, 150, 22);
	Place(s1, maxRows, 764, y + 148, 90, 22);
	Place(s1, lDefRows, 858, y + 148, 110, 22);
	y += 182;

	boxRows.SetLabel("Rows and read-back");
	s1.Add(boxRows.HSizePosZ(8, 8).TopPosZ(y, 72));
	lRb.SetText("Read-back:");
	lPg.SetText("PG test tables after read-back:");
	rb.Add("sample", "count + checksum + sample").Add("all", "every row");
	pg.Add("keep-last", "keep last run").Add("delete", "delete");
	Place(s1, lRb, 20, y + 18, 130, 22);
	Place(s1, rb, 150, y + 18, 360, 22);
	Place(s1, lPg, 20, y + 44, 210, 22);
	Place(s1, pg, 230, y + 44, 260, 22);
	y += 80;

	lCmd.SetText("Command that will run:");
	Place(s1, lCmd, 8, y, 170, 20);
	copyCmd.SetLabel("Copy");
	copyCmd.Tip("Copy the command line(s), each with a cd into its folder, to run them yourself in a terminal");
	Place(s1, copyCmd, 180, y - 1, 70, 22);
	copyCmd << THISBACK(CopyCommand);
	s1.Add(cmd.HSizePosZ(8, 8).VSizePosZ(y + 22, 66));
	cmd.SetReadOnly();
	cmd.SetFont(Monospace(Zy(11)));
	s1.Add(lInfo.HSizePosZ(8, 8).BottomPosZ(40, 22));
	lastResults.SetLabel("Last results");
	saveDef.SetLabel("Save as default");
	start.SetLabel("Start");
	s1.Add(lastResults.LeftPosZ(8, 120).BottomPosZ(8, 26));
	s1.Add(saveDef.RightPosZ(136, 130).BottomPosZ(8, 26));
	s1.Add(start.RightPosZ(8, 120).BottomPosZ(8, 26));
	lastResults << THISBACK(LastResults);
	saveDef << THISBACK(SaveDefault);
	start << THISBACK(Start);
	start.Ok();

	Ctrl *live[] = { &version, &comp, &type, &front, &size, &rb, &pg, &gCtest, &gSmoke, &gBench, &gVerifyLite,
	                 &gVerify, &bStrict, &bHardened, &pSql, &pPgL, &pPgR, &pJt, &pBin, &pJson, &pHtml, &pgHost, &pgPort, &pgDb, &pgUser, &connFile, &maxRuns, &maxRecords, &maxRows };
	for(Ctrl *c : live)
		c->WhenAction << THISBACK(UpdateCommand);

	// ================= Screen 2
	s2.Add(runTitle.HSizePosZ(8, 8).TopPosZ(8, 22));
	s2.Add(prog.HSizePosZ(8, 8).TopPosZ(34, 20));
	s2.Add(counts.LeftPosZ(8, 300).TopPosZ(58, 22));
	s2.Add(elapsed.RightPosZ(8, 200).TopPosZ(58, 22));
	elapsed.AlignRight();
	s2.Add(out.HSizePosZ(8, 8).VSizePosZ(84, 42));
	out.SetReadOnly();
	out.SetFont(Monospace(Zy(11)));
	stop.SetLabel("Stop");
	runBack.SetLabel("Back");
	runResults.SetLabel("Results");
	s2.Add(stop.LeftPosZ(8, 120).BottomPosZ(8, 26));
	s2.Add(runResults.RightPosZ(136, 120).BottomPosZ(8, 26));
	s2.Add(runBack.RightPosZ(8, 120).BottomPosZ(8, 26));
	runReport.SetLabel("Reports");
	runReport.Tip("Show the report pages this run rendered from results.db (test-summary/*.md)");
	s2.Add(runReport.RightPosZ(264, 120).BottomPosZ(8, 26));
	stop << THISBACK(Stop);
	runBack << [this] { Show(1); };
	runResults << [this] { ShowResults(); };
	runReport << THISBACK(ShowReport);
	runChrome.SetLabel("Open Reports in Chrome");
	runChrome.Tip("Open the Group Summary in Chrome as a web page; its links lead to every other report page");
	s2.Add(runChrome.RightPosZ(392, 180).BottomPosZ(8, 26));
	runChrome << THISBACK(OpenReportsInBrowser);

	// ================= Screen 3
	s3.Add(header.HSizePosZ(8, 8).TopPosZ(6, 22));
	header.SetFont(StdFont().Bold());
	lTest.SetText("Test:");
	lBackend.SetText("Backend:");
	Place(s3, lTest, 8, 34, 40, 22);
	Place(s3, fTest, 50, 34, 200, 22);
	Place(s3, lBackend, 264, 34, 60, 22);
	Place(s3, fBackend, 326, 34, 160, 22);
	failOnly.SetLabel("failures only");
	Place(s3, failOnly, 500, 34, 140, 22);
	fTest.WhenAction = fBackend.WhenAction = failOnly.WhenAction = THISBACK(FillList);

	list.AddIndex("IDX");
	list.AddColumn("Test", 18);
	list.AddColumn("Compiler", 8);
	list.AddColumn("Backend", 10);
	list.AddColumn("Rows", 8);
	list.AddColumn("Count", 8);
	list.AddColumn("Checksum", 10);
	list.AddColumn("Sample", 8);
	list.AddColumn("ms", 7);
	list.AddColumn("Ping ms", 9);
	list.AddColumn("Write ms", 9);
	list.AddColumn("Status", 7);
	list.WhenSel = THISBACK(RowSel);

	vTitle.SetText("Select a result row, or Open file...");
	viewer.Add(vTitle.HSizePosZ(4, 170).TopPosZ(0, 22));
	viewMode.Add("table", "Table").Add("raw", "Raw");
	viewMode <<= "table";
	viewMode.WhenAction = THISBACK(ViewMode);
	viewer.Add(viewMode.RightPosZ(4, 130).TopPosZ(0, 22));
	viewer.Add(sectionList.HSizePosZ(4, 4).TopPosZ(24, 22));
	sectionList.WhenAction = THISBACK(ShowSection);
	viewer.Add(table.HSizePosZ(4, 4).VSizePosZ(50, 26));
	viewer.Add(raw.HSizePosZ(4, 4).VSizePosZ(50, 26));
	viewer.Add(validation.HSizePosZ(4, 4).BottomPosZ(2, 22));
	raw.SetReadOnly();
	raw.SetFont(Monospace(Zy(11)));
	raw.Hide();
	sectionList.Hide();
	table.HeaderObject().Absolute();

	split.Horz(list, viewer);
	split.SetPos(6300);
	s3.Add(split.HSizePosZ(8, 8).VSizePosZ(62, 42));
	openFile.SetLabel("Open file...");
	showLog.SetLabel("Show log");
	resBack.SetLabel("Back");
	quit.SetLabel("Quit");
	s3.Add(openFile.LeftPosZ(8, 120).BottomPosZ(8, 26));
	s3.Add(showLog.LeftPosZ(136, 100).BottomPosZ(8, 26));
	copyAi.SetLabel("Copy for AI");
	copyAi.Tip("Copy a Markdown report (project context, what was run, results, failure logs and the "
	           "data on the right) to the clipboard, and save it beside the summary as *_ai.md. "
	           "Paste it into any AI and ask what it means.");
	s3.Add(copyAi.LeftPosZ(244, 120).BottomPosZ(8, 26));
	copyAi << THISBACK(CopyForAi);
	showAi.SetLabel("Show AI md");
	showAi.Tip("Show the Markdown file that Copy for AI saved (enabled once it exists)");
	s3.Add(showAi.LeftPosZ(372, 120).BottomPosZ(8, 26));
	showAi << THISBACK(ShowAi);
	showAi.Disable();
	report.SetLabel("Reports");
	report.Tip("Show the report pages rendered from results.db at the end of every run "
	           "(test-summary/README.md, then each gate's pages); pick a page in the list above");
	s3.Add(report.LeftPosZ(500, 100).BottomPosZ(8, 26));
	report << THISBACK(ShowReport);
	chrome.SetLabel("Open Reports in Chrome");
	chrome.Tip("Open the Group Summary in Chrome as a web page (made from the report pages into "
	           "~/.cache/jac313/reports); its links lead to every other report page");
	s3.Add(chrome.LeftPosZ(608, 180).BottomPosZ(8, 26));
	chrome << THISBACK(OpenReportsInBrowser);
	s3.Add(resBack.RightPosZ(136, 120).BottomPosZ(8, 26));
	s3.Add(quit.RightPosZ(8, 120).BottomPosZ(8, 26));
	openFile << THISBACK(OpenFile);
	showLog << THISBACK(ShowLog);
	resBack << [this] { Show(1); };
	quit << [this] { Close(); };

	LoadSettings(opt);
	if(gCmdVersion.GetCount())
		opt.version = gCmdVersion;
	root <<= EffectiveRoot(opt, gCmdRoot);
	FillVersions();
	FromOptions();
	LoadConn(true);   // fill the Remote PG fields from the connections file when it exists
	UpdateCommand();
	Show(1);
	// raw bytes: the system-charset filter would turn the Unit Separator (NULL) into '_'
	proc.ConvertCharset(false);
	dproc.ConvertCharset(false);
	tick.Set(-50, THISBACK(Poll));
}

Runner::~Runner()
{
	tick.Kill();
	if(running && proc.IsRunning())
		KillGroup(proc, SIGKILL);
	if(dRunning && dproc.IsRunning())
		KillGroup(dproc, SIGKILL);
}

void Runner::Show(int screen)
{
	s1.Show(screen == 1);
	s2.Show(screen == 2);
	s3.Show(screen == 3);
}

// One Version choice per vNNN folder under the root, labelled with its C++ standard.
// Keeps the current choice when it still exists, else selects the newest.
void Runner::FillVersions()
{
	String keep = IsNull(~version) ? opt.version : AsString(~version);
	String r = Root();
	String key;
	for(const VersionInfo& v : FindVersions(r))
		key << v.name << v.standard << ";";
	if(key != versionKey) {   // rebuild only when the set changed (root.WhenAction fires per key)
		versionKey = key;
		version.Reset();
		for(const VersionInfo& v : FindVersions(r))
			version.Add(v.name, v.standard.GetCount() ? v.name + " (" + v.standard + ")" : v.name);
		version.Refresh();
	}
	version <<= ResolveVersion(keep, r);
}

void Runner::FromOptions()
{
	version <<= ResolveVersion(opt.version, Root());
	gCtest <<= opt.ctest;
	gSmoke <<= opt.smoke;
	gBench <<= opt.bench;
	gVerifyLite <<= opt.verifyLite;
	gVerify <<= opt.verify;
	comp <<= opt.compiler;
	type <<= opt.buildType;
	bStrict <<= opt.strict;
	bHardened <<= opt.hardened;
	front <<= opt.frontEnd;
	size <<= opt.size;
	pSql <<= opt.sqlite;
	pPgL <<= opt.pgLocal;
	pPgR <<= opt.pgRemote;
	pJt <<= opt.jtext;
	pBin <<= opt.binary;
	pJson <<= opt.json;
	pHtml <<= opt.html;
	maxRuns <<= opt.maxRuns;
	maxRecords <<= opt.maxRecords;
	maxRows <<= opt.maxRows;
	rb <<= opt.readback;
	pg <<= opt.pgCleanup;
	pgHost <<= opt.pgHost;
	pgPort <<= opt.pgPort;
	pgDb <<= opt.pgDb;
	pgUser <<= opt.pgUser;
	connFile <<= EffectiveConnFile(opt, Root());
}

void Runner::ToOptions()
{
	opt.version = AsString(~version);
	opt.ctest = gCtest;
	opt.smoke = gSmoke;
	opt.bench = gBench;
	opt.verifyLite = gVerifyLite;
	opt.verify = gVerify;
	opt.compiler = AsString(~comp);
	opt.buildType = AsString(~type);
	opt.strict = bStrict;
	opt.hardened = bHardened;
	opt.frontEnd = AsString(~front);
	opt.size = AsString(~size);
	opt.sqlite = pSql;
	opt.pgLocal = pPgL;
	opt.pgRemote = pPgR;
	opt.jtext = pJt;
	opt.binary = pBin;
	opt.json = pJson;
	opt.html = pHtml;
	opt.maxRuns = IsNull(~maxRuns) ? 0 : max((int)~maxRuns, 0);
	opt.maxRecords = IsNull(~maxRecords) ? 0 : max((int)~maxRecords, 0);
	opt.maxRows = IsNull(~maxRows) ? 0 : max((int)~maxRows, 0);
	LimitPersistToRecords(opt);
	opt.readback = AsString(~rb);
	opt.pgCleanup = AsString(~pg);
	opt.pgHost = TrimBoth(~pgHost);
	opt.pgPort = TrimBoth(~pgPort);
	opt.pgDb = TrimBoth(~pgDb);
	opt.pgUser = TrimBoth(~pgUser);
	// keep the path only when it differs from the default, so a moved tree still finds its own
	String cf = TrimBoth(~connFile);
	opt.connFile = (cf == DefaultConnectionsPath(Root())) ? String() : cf;
	// children inherit it: the dataset viewer uses it for summaries that predate "pg_remote"
	if(RemoteConninfo(opt).GetCount())
		setenv("JAC313_PG_REMOTE", WithPassFile(RemoteConninfo(opt), PassFileFor(opt, Root(), true)), 1);
	// pg-local's server for the viewer: the default, with bootstrap's password file when there is one
	if(PassFileFor(opt, Root(), false).GetCount() && !getenv("JAC313_PG_LOCAL"))
		setenv("JAC313_PG_LOCAL", WithPassFile("host=localhost dbname=jac313 user=jac313 connect_timeout=10",
		                                       PassFileFor(opt, Root(), false)), 1);
	// keep the root only when it differs from the one found from the executable,
	// so a moved tree still finds itself
	String r = Root();
	opt.root = (r == FindRootFromExe()) ? String() : r;
}

void Runner::UpdateCommand()
{
	ToOptions();
	ShowCompilerVersions();
	String r = Root();
	String text;
	Array<RunPlan> p = BuildPlans(opt, r);
	for(int i = 0; i < p.GetCount(); i++) {
		if(p.GetCount() > 1)
			text << (i + 1) << ") ";
		text << p[i].Display() << "\n";
	}
	if(p.GetCount())
		text << "(in " << p[0].cwd << (p.GetCount() > 1 ? "; the two runs go one after the other)" : ")");
	cmd <<= text;
	String info;
	if(r.IsEmpty())
		info = "No project root: set one above.";
	else if(p.GetCount() && !FileExists(p[0].exe))
		info = "Not found: " + p[0].exe;
	else if(PersistList(opt).IsEmpty())
		info = "Choose at least one place to persist to.";
	else if(!(opt.ctest || opt.smoke || opt.bench || opt.verifyLite || opt.verify))
		info = "Choose at least one gate.";
	lInfo.SetInk(info.GetCount() ? Color(170, 0, 0) : SColorText());
	lInfo.SetText(info);
}

void Runner::CopyCommand()
{
	ToOptions();
	String text;
	for(const RunPlan& p : BuildPlans(opt, Root()))
		text << "cd " << QuoteArg(p.cwd) << " && " << p.Display() << "\n";
	if(text.IsEmpty())
		return;
	WriteClipboardText(text);
	lInfo.SetInk(SColorText());
	lInfo.SetText("Copied the command line: paste it into a terminal to run it yourself.");
}

void Runner::ShowCompilerVersions()
{
	String dir = AppendFileName(Root(), opt.version);
	if(dir == compVerFor)
		return;
	String exe = AppendFileName(dir, "jac313_test_cli");
	String text;
	LocalProcess p;
	Vector<String> args;
	args << "compiler-versions";
	if(FileExists(exe) && p.Start(exe, args, NULL, dir)) {
		String out, chunk;
		dword t = msecs();
		while(p.IsRunning() && msecs(t) < 10000) {   // Read is true while it runs, data or not
			if(p.Read(chunk))
				out << chunk;
			Sleep(10);
		}
		while(p.Read(chunk) && chunk.GetCount())
			out << chunk;
		if(p.IsRunning())
			p.Kill();
		for(const String& line : Split(out, '\n')) {   // "gcc gcc16 16.2.1"
			Vector<String> w = Split(line, ' ');
			if(w.GetCount() == 3 && (w[0] == "gcc" || w[0] == "clang")) {
				if(text.GetCount())
					text << ",  ";
				text << w[0] << " " << (w[2] == "-" ? String("not found") : w[2]);
			}
		}
	}
	if(text.GetCount())   // asked again next time when the tool is missing or did not answer
		compVerFor = dir;
	lCompVer.SetText(text);
}

void Runner::Browse()
{
	FileSel fs;
	fs.ActiveDir(Root());
	if(fs.ExecuteSelectDir("Project root (the dir holding the vNNN folders)")) {
		root <<= ~fs;
		FillVersions();
		UpdateCommand();
	}
}

void Runner::SaveDefault()
{
	ToOptions();
	maxRows <<= opt.maxRows;
	if(SaveSettings(opt))
		lInfo.SetText("Saved as default: " + SettingsPath());
	else
		Exclamation("Cannot write " + DeQtf(SettingsPath()));
}

void Runner::Start()
{
	if(running)
		return;
	ToOptions();
	maxRows <<= opt.maxRows;
	String r = Root();
	if(r.IsEmpty() || opt.version.IsEmpty() || !DirectoryExists(AppendFileName(r, opt.version))) {
		Exclamation("No " + DeQtf(opt.version) + " under the project root " + DeQtf(r) + ".");
		return;
	}
	if(PersistList(opt).IsEmpty()) {
		Exclamation("Choose at least one place to persist to.");
		return;
	}
	if(!(opt.ctest || opt.smoke || opt.bench || opt.verifyLite || opt.verify)) {
		Exclamation("Choose at least one gate.");
		return;
	}
	if(opt.pgRemote && opt.pgHost.IsEmpty()) {
		Exclamation("PG remote is ticked but no server is set.&Fill in Remote PG (Host, and Database / User "
		            "if they differ), press Test, or untick PG remote.");
		return;
	}
	plans = BuildPlans(opt, r);
	sums.Clear();
	out.Clear();
	planIx = 0;
	running = true;
	stopping = false;
	t0 = msecs();
	stop.Enable();
	runBack.Disable();
	runResults.Disable();
	Show(2);
	StartPlan();
}

void Runner::StartPlan()
{
	const RunPlan& p = plans[planIx];
	nPass = nFail = 0;
	lineBuf.Clear();
	prog.Set(0, 1);
	counts.SetText("pass 0 fail 0");
	runTitle.SetText(Format("Run %d of %d: %s", planIx + 1, plans.GetCount(), p.compiler));
	out.Append(String("$ ") + p.Display() + "\n");
	DeleteFile(p.summary);   // a summary left from an older run must not pass for this one
	if(!StartChild(proc, p.exe, p.args, p.cwd, false)) {
		out.Append("cannot start " + p.exe + "\n");
		PlanFinished();
	}
}

void Runner::Feed(const String& chunk)
{
	String s = StripAnsi(chunk);
	if(out.GetLength() > 4000000)
		out.Remove(0, 1000000);
	out.Append(s);
	out.SetCursor(out.GetLength());
	lineBuf << s;
	int nl;
	while((nl = lineBuf.Find('\n')) >= 0) {
		String line = lineBuf.Left(nl);
		lineBuf.Remove(0, nl + 1);
		int i, n;
		if(ParseProgress(line, i, n)) {
			prog.Set(min(i, n), n);
			if(HasWord(line, "FAIL") || HasWord(line, "FAILED"))
				nFail++;
			else if(HasWord(line, "PASS") || HasWord(line, "PASSED"))
				nPass++;
			counts.SetText(Format("pass %d fail %d", nPass, nFail));
		}
	}
}

void Runner::Poll()
{
	if(running) {
		elapsed.SetText(Format("elapsed %s", Elapsed(msecs() - t0)));
		String s;
		bool alive = Drain(proc, s, NULL);
		if(s.GetCount())
			Feed(s);
		if(stopping && msecs(stopAt) > 3000 && proc.IsRunning())
			KillGroup(proc, SIGKILL);
		if(!alive)
			PlanFinished();
	}
	if(dRunning) {
		bool alive = Drain(dproc, dOut, &dErr);
		if(!alive) {
			dRunning = false;
			int code = dproc.GetExitCode();
			String vline;
			Vector<String> el = Split(dErr, '\n');
			for(int i = el.GetCount() - 1; i >= 0; i--)
				if(el[i].Find("VALIDATION") >= 0) {
					vline = TrimBoth(el[i]);
					break;
				}
			if(vline.IsEmpty())
				vline = Format("exit code %d", code) + (el.GetCount() ? ": " + TrimBoth(el.Top()) : String());
			ShowDoc(dOut, Null);
			bool bad = code != 0 || (vline.Find("errors=") >= 0 && vline.Find("errors=0") < 0);
			validation.SetInk(bad ? Color(170, 0, 0) : Color(0, 120, 0));
			validation.SetText(vline);
		}
	}
}

void Runner::PlanFinished()
{
	if(lineBuf.GetCount())
		Feed("\n");
	int code = proc.GetExitCode();
	const RunPlan& p = plans[planIx];
	// LocalProcess reports a child ended by signal N as 1000 + N
	out.Append(code >= 1000 ? Format("[%s ended by signal %d]\n", p.compiler, code - 1000)
	                        : Format("[%s finished, exit code %d]\n", p.compiler, code));
	Summary& s = sums.Add();
	String err;
	if(!s.Load(p.summary, err)) {
		sums.Drop();
		out.Append("[" + err + "]\n");
	}
	else
		out.Append(Format("[summary %s: pass %d fail %d, %d rows]\n", p.summary, s.pass, s.fail, s.rows.GetCount()));
	out.SetCursor(out.GetLength());
	if(!stopping && planIx + 1 < plans.GetCount()) {
		planIx++;
		StartPlan();
		return;
	}
	AllFinished();
}

void Runner::AllFinished()
{
	running = false;
	stop.Disable();
	runBack.Enable();
	runResults.Enable(sums.GetCount());
	runReport.Enable(FileExists(AppendFileName(ReportDir(), "README.md")));
	runChrome.Enable(runReport.IsEnabled());
	runTitle.SetText(stopping ? "Stopped" : "Finished");
	elapsed.SetText(Format("elapsed %s", Elapsed(msecs() - t0)));
	if(!stopping && sums.GetCount())
		ShowResults();
}

void Runner::Stop()
{
	if(!running || !proc.IsRunning())
		return;
	stopping = true;
	stopAt = msecs();
	out.Append("[Stop: SIGTERM to the process group]\n");
	KillGroup(proc, SIGTERM);
}

void Runner::LastResults()
{
	ToOptions();
	String dir = AppendFileName(AppendFileName(Root(), opt.version), "test-summary");
	Vector<String> files;
	for(const char *f : { "runner_last.json", "runner_last_gcc.json", "runner_last_clang.json" })
		if(FileExists(AppendFileName(dir, f)))
			files.Add(AppendFileName(dir, f));
	if(files.IsEmpty()) {
		FileSel fs;
		fs.Type("Summary JSON", "*.json").AllFilesType();
		fs.ActiveDir(dir);
		if(!fs.ExecuteOpen("Open a summary JSON"))
			return;
		files.Add(~fs);
	}
	String err;
	if(!LoadSummaries(files, err)) {
		Exclamation(DeQtf(err));
		return;
	}
	ShowResults();
}

bool Runner::LoadSummaries(const Vector<String>& files, String& err)
{
	sums.Clear();
	for(const String& f : files) {
		Summary& s = sums.Add();
		if(!s.Load(f, err)) {
			sums.Clear();
			return false;
		}
	}
	return true;
}

void Runner::ShowResults()
{
	all.Clear();
	aiPath.Clear();          // a new set of results has no AI report until Copy for AI makes one
	showAi.Disable();
	String h;
	for(int i = 0; i < sums.GetCount(); i++) {
		const Summary& s = sums[i];
		if(i)
			h << "   |   ";
		h << "run " << Dash(s.runId) << " · " << Dash(s.host) << " · " << Dash(s.version) << " · "
		  << Dash(s.compiler) << " " << Dash(s.buildType) << " · pass " << s.pass << " fail " << s.fail
		  << " (" << s.rows.GetCount() << " rows)";
		for(const ResultRow& r : s.rows)
			all.Add(&r);
	}
	header.SetText(h);
	Index<String> tests, backends;
	for(const ResultRow *r : all) {
		tests.FindAdd(r->test);
		backends.FindAdd(r->backend);
	}
	fTest.ClearList();
	fBackend.ClearList();
	fTest.Add(Null, "(all)");
	fBackend.Add(Null, "(all)");
	for(const String& t : tests)
		fTest.Add(t, Dash(t));
	for(const String& b : backends)
		fBackend.Add(b, Dash(b));
	fTest <<= Null;
	fBackend <<= Null;
	list.HeaderObject().ShowTab(1, sums.GetCount() > 1);
	FillList();
	Show(3);
}

static Value Mark(const String& s)
{
	String u = ToUpper(s);
	if(u == "PASS")
		return AttrText("\xE2\x9C\x94").Ink(Color(0, 140, 0));
	if(u.IsEmpty())
		return "-";
	if(u == "FAIL")
		return AttrText("\xE2\x9C\x98").Ink(Color(190, 0, 0));
	return AttrText(s).Ink(Color(150, 90, 0));
}

void Runner::FillList()
{
	String ft = ~fTest, fb = ~fBackend;
	bool fo = failOnly;
	list.Clear();
	for(int i = 0; i < all.GetCount(); i++) {
		const ResultRow& r = *all[i];
		if(ft.GetCount() && r.test != ft)
			continue;
		if(fb.GetCount() && r.backend != fb)
			continue;
		if(fo && !r.Failed())
			continue;
		list.Add(i, Dash(r.test), Dash(r.compiler), Dash(r.backend), Dash(r.rows), Mark(r.rbCount),
		         Mark(r.rbChecksum), Mark(r.rbSample), Dash(r.ms), Dash(r.pingMs), Dash(r.writeMs),
		         r.Failed() ? AttrText(ToUpper(r.status) == "FAIL" || r.status.IsEmpty() ? "\xE2\x9C\x98" : ~r.status).Ink(Color(190, 0, 0))
		                    : AttrText("\xE2\x9C\x94").Ink(Color(0, 140, 0)));
	}
}

void Runner::RowSel()
{
	if(!list.IsCursor())
		return;
	int i = list.GetKey();
	if(i < 0 || i >= all.GetCount())
		return;
	const ResultRow& r = *all[i];
	if(dRunning) {
		KillGroup(dproc, SIGKILL);
		dproc.Kill();
		dRunning = false;
	}
	showLog.Enable(r.log.GetCount());
	vTitle.SetText(Dash(r.test) + " · " + Dash(r.backend) + (r.outputMode.GetCount() ? " · " + r.outputMode : String())
	              + (r.error.GetCount() ? " · error: " + r.error : String()));
	String exe;
	Vector<String> args;
	validation.SetInk(SColorText());
	if(!DatasetCommand(r, Root(), exe, args)) {
		ShowDoc(Null, Null);
		validation.SetText("No dataset for this row.");
		return;
	}
	dCmd = QuoteArg(exe);
	for(const String& a : args)
		dCmd << ' ' << QuoteArg(a);
	if(!FileExists(exe)) {
		ShowDoc(Null, Null);
		validation.SetInk(Color(170, 0, 0));
		validation.SetText("Dataset tool not found: " + exe);
		return;
	}
	dOut.Clear();
	dErr.Clear();
	ShowDoc(Null, Null);
	validation.SetText("Reading: " + dCmd);
	if(!StartChild(dproc, exe, args, GetFileFolder(exe), true)) {
		validation.SetInk(Color(170, 0, 0));
		validation.SetText("Cannot start: " + dCmd);
		return;
	}
	dRunning = true;
}

void Runner::ShowDoc(const String& text, const String& baseDir)
{
	reportPages.Clear();   // the section list holds jText sections again
	docText = text;
	doc.Parse(text, baseDir);
	raw.Set(text);
	sectionList.ClearList();
	for(int i = 0; i < doc.sections.GetCount(); i++)
		sectionList.Add(i, Format("Section: %s (%d rows)", doc.sections[i].name, doc.sections[i].rows.GetCount()));
	int m = doc.MainSection();
	if(m >= 0)
		sectionList <<= m;
	sectionList.Show(doc.sections.GetCount() > 1);
	ShowSection();
}

void Runner::ShowSection()
{
	if(reportPages.GetCount()) {
		ShowReportPage();
		return;
	}
	table.Reset();
	table.HeaderObject().Absolute();
	int si = IsNull(~sectionList) ? -1 : (int)~sectionList;
	if(si < 0 || si >= doc.sections.GetCount())
		return;
	const JSection& s = doc.sections[si];
	int w = s.Width();
	Font f = StdFont();
	for(int c = 0; c < w; c++) {
		int cx = GetTextSize(s.ColName(c), f).cx + DPI(16);
		for(int r = 0; r < min(200, s.rows.GetCount()); r++)
			if(c < s.rows[r].GetCount())
				cx = max(cx, GetTextSize(s.rows[r][c].Left(60), f).cx + DPI(12));
		table.AddColumn(s.ColName(c), min(cx, DPI(320)));
	}
	for(const auto& row : s.rows) {
		Vector<Value> v;
		for(int c = 0; c < w; c++)
			v.Add(c < row.GetCount() ? row[c] : JNull());
		table.Add(v);
	}
}

void Runner::ViewMode()
{
	bool rawMode = AsString(~viewMode) == "raw";
	raw.Show(rawMode);
	table.Show(!rawMode);
}

void Runner::OpenJText(const String& file)
{
	String text = LoadFile(file);
	if(text.IsVoid()) {
		Exclamation("Cannot read " + DeQtf(file));
		return;
	}
	vTitle.SetText(GetFileName(file));
	ShowDoc(text, GetFileFolder(file));
	int si = doc.MainSection();
	String v = si >= 0 ? Format("file: %d columns, %d rows (read by this window; no engine validation)",
	                            doc.sections[si].Width(), doc.sections[si].rows.GetCount())
	                   : String("file: no sections found");
	if(doc.notes.GetCount())
		v << " · " << doc.notes[0] << (doc.notes.GetCount() > 1 ? Format(" (+%d more)", doc.notes.GetCount() - 1) : String());
	validation.SetInk(doc.notes.GetCount() ? Color(150, 90, 0) : SColorText());
	validation.SetText(v);
}

void Runner::OpenFile()
{
	FileSel fs;
	fs.Type("jText", "*.jtext;*.jtxt").AllFilesType();
	if(fs.ExecuteOpen("Open a jText file"))
		OpenJText(~fs);
}

void Runner::ShowLog()
{
	if(!list.IsCursor())
		return;
	const ResultRow& r = *all[(int)list.GetKey()];
	if(r.log.IsEmpty() || !FileExists(r.log)) {
		Exclamation("No log file " + DeQtf(r.log));
		return;
	}
	vTitle.SetText("log: " + GetFileName(r.log));
	ShowDoc(Null, Null);
	raw.Set(LoadFile(r.log));
	viewMode <<= "raw";
	ViewMode();
	validation.SetInk(SColorText());
	validation.SetText(r.log);
}

void Runner::CopyForAi()
{
	if(sums.IsEmpty()) {
		Exclamation("No results loaded.");
		return;
	}
	AiExtra x;
	// Our AI report open in the viewer: close it, rebuild, and open the new one in its place,
	// so the old report is never nested in the new one and nothing stale stays on screen.
	bool showingAi = String(vTitle.GetText()).StartsWith("AI report:");
	if(showingAi) {
		vTitle.SetText("");
		ShowDoc(Null, Null);
		raw.Clear();
	}
	if(!showingAi && (docText.GetCount() || raw.GetLength())) {
		x.title = vTitle.GetText();
		x.datasetText = docText.GetCount() ? docText : raw.Get();
		x.validation = validation.GetText();
	}
	String md = BuildAiReport(sums, Root(), x);
	WriteClipboardText(md);
	String path = AiReportPath(sums[0]);
	bool saved = SaveFile(path, md);
	aiPath = saved ? path : String();
	showAi.Enable(saved);
	if(showingAi && saved)
		ShowAi();   // reopen the rebuilt report where the old one was
	validation.SetInk(saved ? SColorText() : Color(170, 0, 0));
	validation.SetText(Format("Copied for AI (%d KB) to the clipboard", md.GetCount() / 1024 + 1)
	                   + (saved ? String(showingAi ? ", rebuilt and reopened: " : " and saved to ") + path
	                            : String(" (could not save ") + path + ")"));
}

void Runner::ShowAi()
{
	if(aiPath.IsEmpty() || !FileExists(aiPath)) {
		Exclamation("No AI report file yet: press Copy for AI first.");
		showAi.Disable();
		return;
	}
	vTitle.SetText("AI report: " + GetFileName(aiPath));
	ShowDoc(Null, Null);
	raw.Set(LoadFile(aiPath));
	viewMode <<= "raw";
	ViewMode();
	validation.SetInk(SColorText());
	validation.SetText(aiPath);
}

String Runner::ReportDir()
{
	ToOptions();
	return AppendFileName(AppendFileName(Root(), opt.version), "test-summary");
}

// Reports: the pages jac313_test_cli --report renders at the end of every run. The list above
// the viewer holds the pages: Group Summary (test-summary/README.md) first, then per gate its
// README and its runs, newest first.
void Runner::ShowReport()
{
	String dir = ReportDir();
	if(!FileExists(AppendFileName(dir, "README.md"))) {
		Exclamation(DeQtf("No report pages yet in " + dir + ": they are rendered at the end of a run."));
		return;
	}
	if(dRunning) {
		KillGroup(dproc, SIGKILL);
		dproc.Kill();
		dRunning = false;
	}
	ShowDoc(Null, Null);
	Vector<String> labels;
	reportPages.Add(AppendFileName(dir, "README.md"));
	labels.Add("Group Summary");
	Vector<String> gates;
	for(FindFile ff(AppendFileName(dir, "*")); ff; ff.Next())
		if(ff.IsFolder() && FileExists(AppendFileName(ff.GetPath(), "README.md")))
			gates.Add(ff.GetName());
	Sort(gates);
	for(const String& g : gates) {
		String gd = AppendFileName(dir, g);
		reportPages.Add(AppendFileName(gd, "README.md"));
		labels.Add(g + " summary");
		Vector<String> runs;
		for(FindFile ff(AppendFileName(gd, "Run_*.md")); ff; ff.Next())
			if(ff.IsFile())
				runs.Add(ff.GetName());
		Sort(runs, std::greater<String>());   // Run_030 before Run_024
		for(const String& r : runs) {
			reportPages.Add(AppendFileName(gd, r));
			labels.Add(g + " · " + GetFileTitle(r));
		}
	}
	sectionList.ClearList();
	for(int i = 0; i < labels.GetCount(); i++)
		sectionList.Add(i, Format("Page %d of %d: %s", i + 1, labels.GetCount(), labels[i]));
	sectionList <<= 0;
	sectionList.Show();
	viewMode <<= "raw";
	ViewMode();
	Show(3);
	ShowReportPage();
}

void Runner::ShowReportPage()
{
	int i = IsNull(~sectionList) ? 0 : (int)~sectionList;
	if(i < 0 || i >= reportPages.GetCount())
		return;
	const String& page = reportPages[i];
	String dir = ReportDir();
	vTitle.SetText("Reports: " + (i == 0 ? String("Group Summary") : page.Mid(dir.GetCount() + 1)));
	raw.Set(LoadFile(page));
	validation.SetInk(SColorText());
	validation.SetText(page);
}

// Open Reports in Chrome: the report pages turned into HTML (links .md -> .html) in
// ~/.cache/jac313/reports/<root>_<version>/, then the Group Summary opened in the browser.
void Runner::OpenReportsInBrowser()
{
	String src = ReportDir();
	if(!FileExists(AppendFileName(src, "README.md"))) {
		Exclamation(DeQtf("No report pages yet in " + src + ": they are rendered at the end of a run."));
		return;
	}
	String key = Root() + "_" + opt.version;
	for(int i = 0; i < key.GetCount(); i++)
		if(!IsAlNum(key[i]) && key[i] != '-' && key[i] != '.')
			key.Set(i, '_');
	String dst = AppendFileName(GetHomeDirFile(".cache/jac313/reports"), TrimLeft("_", key));
	String err;
	int n = ExportReportHtml(src, dst, err);
	if(n == 0) {
		Exclamation(DeQtf("Could not make the report web pages: " + err));
		return;
	}
	String page = AppendFileName(dst, "README.html");
	String browser = FindBrowser();
	bool ok = browser.GetCount() && LaunchDetached(Vector<String>{ browser, page });
	validation.SetInk(ok ? SColorText() : Color(170, 0, 0));
	validation.SetText(ok ? Format("Opened %s in %s (%d pages)", page, GetFileName(browser), n)
	                      : "No browser found (google-chrome, chromium, xdg-open); the pages are in " + dst);
	if(!ok)
		Exclamation(DeQtf(validation.GetText()));   // also seen from the Running screen
}

// Test for PG remote (the Remote PG fields) or PG local (the connections file's pg-local row,
// re-read now so a hand edit counts; else the default local server).
void Runner::TestPg(bool remote)
{
	ToOptions();
	Label&  res = pgTestRes;   // one result word for both buttons; the detail line names which
	String c;
	if(remote)
		c = RemoteConninfo(opt);
	else {
		Vector<ConnRow> rows;
		String err;
		LoadConnections(EffectiveConnFile(opt, Root()), rows, err);   // no file -> the default
		c = LocalConninfo(rows);
	}
	if(c.IsEmpty()) {
		res.SetInk(Color(170, 0, 0));
		res.SetText("Enter the Host first.");
		return;
	}
	String exe = FindDatasetTool(Root(), opt.version);
	if(exe.IsEmpty()) {
		TestResult("Press Start once first: its build makes jac313_store_dataset, which does this test.",
		           false);
		return;
	}
	res.SetInk(SColorText());
	res.SetText(remote ? "Remote: connecting..." : "Local: connecting...");
	pgTest.Disable();
	pgLTest.Disable();
	Sync();
	LocalProcess p;
	Vector<String> args;
	args << "--ping" << (remote ? "pg-remote" : "pg-local")
	     << "--conninfo" << WithPassFile(c, PassFileFor(opt, Root(), remote));
	String out;
	if(p.Start(exe, args)) {
		dword t = msecs();
		String chunk;
		while(p.IsRunning() && msecs(t) < 15000) {   // connect_timeout=10 inside
			if(p.Read(chunk))
				out << chunk;
			ProcessEvents();
			Sleep(20);
		}
		while(p.Read(chunk) && chunk.GetCount())
			out << chunk;
		if(p.IsRunning()) {
			p.Kill();
			out = "PING failed: no answer in 15 s";
		}
	}
	else
		out = "PING failed: cannot start " + exe;
	pgTest.Enable();
	pgLTest.Enable();
	out = TrimBoth(out);
	if(out.StartsWith("PING ok")) {
		// "PING ok connect_ms=19.3 ping_ms=2.182 server=17.10 (...)"
		auto field = [&](const char *k) {
			int a = out.Find(k);
			if(a < 0) return String("?");
			a += (int)strlen(k);
			int e = out.Find(' ', a);
			return e < 0 ? out.Mid(a) : out.Mid(a, e - a);
		};
		int sv = out.Find("server=");
		TestResult(String(remote ? "PG remote" : "PG local") + " OK: connect " + field("connect_ms=") + " ms, ping " + field("ping_ms=")
		           + " ms, PostgreSQL " + (sv >= 0 ? field("server=") : String("?")), true);   // "17.10" (no distro text)
	}
	else
		TestResult(String(remote ? "PG remote: " : "PG local: ")
		                + (out.StartsWith("PING failed: ") ? out.Mid(13) : out), false);
}

// The Test result: a short word beside the button, the whole message on the full-width line
// below it (a PostgreSQL error is longer than the space beside the button), and as a tooltip.
void Runner::TestResult(const String& text, bool ok)
{
	String t = TrimBoth(text);
	t.Replace("\n", " ");
	String who = t.StartsWith("PG local") ? "Local: " : t.StartsWith("PG remote") ? "Remote: " : "";
	pgTestRes.SetInk(ok ? Color(0, 120, 0) : Color(170, 0, 0));
	pgTestRes.SetText(who + (ok ? "OK (details below)" : "failed (see below)"));
	pgTestRes.Tip(t);
	ConnStatus(t, !ok);
	connStatus.Tip(t);
}

void Runner::ConnStatus(const String& text, bool bad)
{
	connStatus.SetInk(bad ? Color(170, 0, 0) : SColorText());
	connStatus.SetText(text);
}

String Runner::CheckConnFile(const String& path)
{
	// jText's own parser + validator, through this version's CLI ("connections --file P")
	String exe = AppendFileName(AppendFileName(Root(), opt.version), "jac313_test_cli");
	if(!FileExists(exe))
		return Null;
	LocalProcess p;
	Vector<String> args;
	args << "connections" << "--file" << path;
	String out, chunk;
	if(!p.Start(exe, args))
		return Null;
	dword t = msecs();
	while(p.IsRunning() && msecs(t) < 10000) {
		if(p.Read(chunk))
			out << chunk;
		Sleep(10);
	}
	while(p.Read(chunk) && chunk.GetCount())
		out << chunk;
	for(const String& l : Split(out, '\n'))
		if(l.StartsWith("VALIDATION"))
			return TrimBoth(l);
	return Null;
}

void Runner::LoadConn(bool quiet)
{
	ToOptions();
	String path = EffectiveConnFile(opt, Root());
	String git = EnclosingGitProject(path);
	String err;
	if(!LoadConnections(path, connRows, err)) {
		if(!FileExists(path))
			ConnStatus(quiet ? "No connections file yet: fill in Remote PG and press Save to create " + path
			                 : "Not found: " + path, !quiet);
		else
			ConnStatus("Cannot read " + path + ": " + err, true);
		return;
	}
	for(const ConnRow& r : connRows)
		if(r.backend == "pg-remote") {
			pgHost <<= r.host;
			pgPort <<= (r.port == "5432" ? String() : r.port);
			pgDb <<= r.dbname;
			pgUser <<= r.user;
		}
	String check = CheckConnFile(path);
	bool bad = git.GetCount() || (check.GetCount() && check.Find("errors=0") < 0);
	ConnStatus(Format("Loaded %d row(s) from %s", connRows.GetCount(), path)
	           + (check.GetCount() ? " · jText " + check : String())
	           + (git.GetCount() ? " · WARNING: inside the git project " + git + ": move it out" : String()), bad);
	UpdateCommand();
}

void Runner::ViewConn()
{
	ToOptions();
	String path = EffectiveConnFile(opt, Root());
	if(!FileExists(path)) {
		ConnStatus("Not found: " + path + " (fill in Remote PG and press Save to create it)", true);
		return;
	}
	Show(3);
	OpenJText(path);
}

void Runner::SaveConn()
{
	ToOptions();
	String path = EffectiveConnFile(opt, Root());
	if(opt.pgHost.IsEmpty()) {
		ConnStatus("Enter the remote server's Host before saving.", true);
		return;
	}
	String git = EnclosingGitProject(path);
	if(git.GetCount() && !PromptYesNo("[* " + DeQtf(path) + "]&is inside the git project " + DeQtf(git)
	                                  + ", so it could be committed.&&Save there anyway?"))
		return;
	Vector<ConnRow> rows;
	if(FileExists(path)) {
		String err;
		LoadConnections(path, rows, err);   // keep the other rows (pg-local, ...)
	}
	bool haveLocal = false;
	int remote = -1;
	for(int i = 0; i < rows.GetCount(); i++) {
		if(rows[i].backend == "pg-remote")
			remote = i;
		if(rows[i].backend == "pg-local")
			haveLocal = true;
	}
	if(!haveLocal) {
		ConnRow& l = rows.Add();
		l.backend = "pg-local";
		l.host = "localhost";
		l.dbname = "jac313";
		l.user = "jac313";
		l.notes = "this machine";
	}
	ConnRow& r = remote >= 0 ? rows[remote] : rows.Add();
	r.backend = "pg-remote";
	r.host = opt.pgHost;
	r.port = opt.pgPort;
	r.dbname = opt.pgDb;
	r.user = opt.pgUser;
	String err;
	if(!SaveConnections(path, rows, err)) {
		ConnStatus("Not saved: " + err, true);
		return;
	}
	connRows = clone(rows);
	String check = CheckConnFile(path);
	ConnStatus("Saved " + path + (check.GetCount() ? " · jText " + check : String())
	           + (git.GetCount() ? " · WARNING: inside the git project " + git : String()),
	           git.GetCount() || (check.GetCount() && check.Find("errors=0") < 0));
	UpdateCommand();
}

void Runner::BrowseConn()
{
	FileSel fs;
	fs.Type("jText files", "*.jtext").AllFilesType();
	String cur = TrimBoth(~connFile);
	fs.ActiveDir(DirectoryExists(GetFileFolder(cur)) ? GetFileFolder(cur) : GetHomeDirectory());
	if(FileExists(cur))
		fs <<= cur;
	if(fs.ExecuteOpen("Connections file (keep it outside git)")) {
		connFile <<= ~fs;
		LoadConn(false);
	}
}

void Runner::Close()
{
	if(running && proc.IsRunning()) {
		if(!PromptYesNo("A run is in progress. Stop it and quit?"))
			return;
		KillGroup(proc, SIGTERM);
		Sleep(300);
		if(proc.IsRunning())
			KillGroup(proc, SIGKILL);
	}
	TopWindow::Close();
}

// ======================================================================= main

static void Settle()
{
	for(int i = 0; i < 25; i++) {
		Ctrl::ProcessEvents();
		Sleep(20);
	}
}

static void Snap(Runner& w, const String& file)
{
	Settle();
	ImageDraw iw(w.GetSize());
	w.DrawCtrl(iw, 0, 0);
	PNGEncoder().SaveFile(file, iw);
	Cout() << "wrote " << file << "\n";
}

GUI_APP_MAIN
{
	String summary, jtext, snap;
	bool autostart = false, copyAiStep = false, reportsStep = false, testRemote = false, testLocal = false, allowRoot = false;
	String connSaveHost;
	int autostop = -1, selectRow = 1;
	const Vector<String>& a = CommandLine();
	for(int i = 0; i < a.GetCount(); i++) {
		if(a[i] == "--root" && i + 1 < a.GetCount())
			gCmdRoot = a[++i];
		else if(a[i] == "--version" && i + 1 < a.GetCount())
			gCmdVersion = a[++i];
		else if(a[i] == "--summary" && i + 1 < a.GetCount())
			summary = a[++i];
		else if(a[i] == "--jtext" && i + 1 < a.GetCount())
			jtext = a[++i];
		else if(a[i] == "--snap" && i + 1 < a.GetCount())
			snap = a[++i];
		else if(a[i] == "--autostart")
			autostart = true;
		else if(a[i] == "--autostop-after" && i + 1 < a.GetCount())
			autostop = ScanInt(a[++i]);
		else if(a[i] == "--select" && i + 1 < a.GetCount())
			selectRow = ScanInt(a[++i]);
		else if(a[i] == "--copy-ai")   // with --snap: also press Copy for AI, then Show AI md
			copyAiStep = true;
		else if(a[i] == "--reports")   // with --snap: also press Reports, then pick page 2
			reportsStep = true;
		else if(a[i] == "--test-remote")   // with --snap: also press Remote PG's Test button
			testRemote = true;
		else if(a[i] == "--test-local")    // with --snap: also press PG local's Test button
			testLocal = true;
		else if(a[i] == "--conn-save" && i + 1 < a.GetCount())   // with --snap: type HOST, press Save
			connSaveHost = a[++i];
		else if(a[i] == "--allow-root")   // run even as root (jac313_test_cli refuses root too)
			allowRoot = true;
	}
	// Root in a tree a regular user owns leaves root-owned trees and logs, and valgrind will not
	// start binaries root has no "other" execute bit on. A root-owned tree (container, CI) is fine.
	// The tree owner is the owner of this program's folder (<root>/gui/out). JAC313_ALLOW_ROOT
	// passes the override on to every jac313_test_cli this window starts.
	struct stat st;
	if(geteuid() == 0 && stat(GetFileFolder(GetExeFilePath()), &st) == 0 && st.st_uid != 0) {
		if(!allowRoot) {
			Exclamation(DeQtf("Jac313Runner should not run as root in a tree a regular user owns. "
			                  "Start it as that user (no sudo).") +
			            "&&" +
			            DeQtf("Root runs leave root-owned build trees, logs and /tmp files that the user's "
			                  "later runs cannot overwrite, and valgrind will not start binaries root has no "
			                  "\"other\" execute bit on.") +
			            "&&" + DeQtf("Start with --allow-root to override."));
			return;
		}
		setenv("JAC313_ALLOW_ROOT", "1", 1);
	}
	Runner w;
	if(summary.GetCount()) {
		Vector<String> f;
		f << summary;
		String err;
		if(w.LoadSummaries(f, err)) {
			w.ShowResults();
			if(w.list.GetCount())
				w.list.SetCursor(0);
			if(jtext.GetCount())
				w.OpenJText(jtext);
		}
		else
			Cerr() << "Jac313Runner: " << err << "\n";
	}
	if(snap.IsEmpty()) {
		w.Run();
		return;
	}
	RealizeDirectory(snap);
	if(autostart) {
		// --snap DIR --autostart [--autostop-after MS] [--select ROW]: press Start with the
		// saved/default options, wait for the run (or Stop it), picture the screens, exit
		w.Open();
		w.Start();
		dword t = msecs();
		bool stopped = false;
		while(w.running && msecs(t) < 120000) {
			Ctrl::ProcessEvents();
			Sleep(20);
			if(autostop >= 0 && !stopped && msecs(t) > autostop) {
				Snap(w, AppendFileName(snap, "run_screen2_before_stop.png"));
				w.Stop();
				stopped = true;
			}
		}
		Cout() << "running=" << w.running << " summaries=" << w.sums.GetCount() << " elapsed_ms=" << msecs(t) << "\n";
		Snap(w, AppendFileName(snap, "run_screen2.png"));
		if(w.sums.GetCount()) {
			w.ShowResults();
			if(selectRow >= 0 && selectRow < w.list.GetCount())
				w.list.SetCursor(selectRow);
			t = msecs();
			while(w.dRunning && msecs(t) < 20000) {
				Ctrl::ProcessEvents();
				Sleep(20);
			}
			Cout() << "viewer: " << w.vTitle.GetText() << " | " << w.validation.GetText() << " | table rows=" << w.table.GetCount() << "\n";
			Snap(w, AppendFileName(snap, "run_screen3.png"));
		}
		w.Close();
		return;
	}
	// --snap DIR: picture each screen, then exit (layout check without a person)
	w.Open();
	w.Show(1);
	Cout() << "conn_file: " << AsString(~w.connFile) << "\n";
	Cout() << "conn_status: " << w.connStatus.GetText() << "\n";
	Cout() << "remote_fields: host=" << AsString(~w.pgHost) << " port=" << AsString(~w.pgPort)
	       << " db=" << AsString(~w.pgDb) << " user=" << AsString(~w.pgUser) << "\n";
	if(connSaveHost.GetCount()) {
		w.pgHost <<= connSaveHost;
		w.SaveConn();
		Cout() << "after_save: " << w.connStatus.GetText() << "\n";
	}
	if(testRemote) {
		w.TestRemote();
		Cout() << "test_remote: " << w.pgTestRes.GetText() << "\n";
	}
	if(testLocal) {
		w.TestLocal();
		Cout() << "test_local: " << w.pgTestRes.GetText() << " | " << w.connStatus.GetText() << "\n";
	}
	Snap(w, AppendFileName(snap, "screen1_options.png"));
	w.Show(2);
	w.runTitle.SetText("Run 1 of 1: gcc (snapshot: sample lines, no child process)");
	w.Feed("$ ./jac313_test_cli --smoke --gcc ...\n"
	       "  [115/172] 005_XS | sqlite | off ... PASS (402 ms)\n"
	       "  [116/172] 006_TS | sqlite | off ... PASS (812 ms)\n"
	       "  [117/172] 006_TS | jtext | off ... FAIL (90 ms)\n");
	w.elapsed.SetText("elapsed 0:01:23");
	Snap(w, AppendFileName(snap, "screen2_running.png"));
	w.Show(3);
	Snap(w, AppendFileName(snap, "screen3_results_table.png"));
	w.viewMode <<= "raw";
	w.ViewMode();
	Snap(w, AppendFileName(snap, "screen3_results_raw.png"));
	if(copyAiStep) {
		Cout() << "show_ai_enabled_before=" << w.showAi.IsEnabled() << "\n";
		w.CopyForAi();
		Cout() << "after copy: " << w.validation.GetText() << " | show_ai_enabled=" << w.showAi.IsEnabled() << "\n";
		Snap(w, AppendFileName(snap, "screen3_ai_copied.png"));
		w.ShowAi();
		Cout() << "viewer: " << w.vTitle.GetText() << "\n";
		Snap(w, AppendFileName(snap, "screen3_ai_shown.png"));
		Sleep(1100);     // so the rebuilt file's mtime differs
		Time before = FileGetTime(w.aiPath);
		w.CopyForAi();   // again, with the report open: close it, rebuild, reopen
		Cout() << "nested_report=" << (LoadFile(w.aiPath).Find("AI report:") >= 0)
		       << " rebuilt=" << (FileGetTime(w.aiPath) > before)
		       << " viewer_matches_file=" << (w.raw.Get() == LoadFile(w.aiPath))
		       << " | " << w.vTitle.GetText() << " | " << w.validation.GetText() << "\n";
		Snap(w, AppendFileName(snap, "screen3_ai_rebuilt.png"));
	}
	if(reportsStep) {
		w.ShowReport();
		Cout() << "reports: pages=" << w.reportPages.GetCount() << " | " << w.vTitle.GetText()
		       << " | viewer_matches_file=" << (w.reportPages.GetCount() && w.raw.Get() == LoadFile(w.reportPages[0])) << "\n";
		Snap(w, AppendFileName(snap, "screen3_reports.png"));
		if(w.reportPages.GetCount() > 1) {
			w.sectionList <<= 1;
			w.ShowSection();
			Cout() << "reports page 2: " << w.vTitle.GetText()
			       << " | viewer_matches_file=" << (w.raw.Get() == LoadFile(w.reportPages[1])) << "\n";
			Snap(w, AppendFileName(snap, "screen3_reports_page2.png"));
		}
		String err;
		String hdir = AppendFileName(snap, "reports_html");
		int n = ExportReportHtml(w.ReportDir(), hdir, err);
		Cout() << "reports html: pages=" << n << " " << err << " | " << AppendFileName(hdir, "README.html") << "\n";
	}
	w.Close();
}
