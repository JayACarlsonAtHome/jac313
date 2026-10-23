// Model.h — everything Jac313Runner knows without a window: the run options and
// the command line they make, the summary JSON the engine writes, the dataset
// command for a result row, and a small tolerant jText reader for the viewer.
// The headless modes (--dump-command, --dump-jtext, --dump-summary) use only this.
#pragma once
#include <Core/Core.h>

namespace Upp {

// ---- Options (Screen 1), saved to Jac313Runner.json beside the program ----
struct RunOptions {
	String root;                    // project root (jac315 dir); "" = found from the executable
	String version;                 // "v001", "v002", ... ("" or not found = the newest bootstrapped version)
	bool   ctest      = true;
	bool   smoke      = true;
	bool   bench      = false;
	bool   verifyLite = false;
	bool   verify     = false;
	String compiler   = "gcc15";    // gcc15 | clang | both
	String buildType  = "Debug";    // Debug | Release
	bool   strict     = false;      // --strict   (extra compiler warnings)
	bool   hardened   = false;      // --hardened (runtime hardening; ctest/smoke builds only)
	String frontEnd   = "textual";  // textual | modules
	String size       = "smoke";    // smoke | full
	bool   sqlite     = true;
	bool   pgLocal    = false;
	bool   pgRemote   = false;
	bool   jtext      = true;
	bool   binary     = true;
	bool   json       = true;
	bool   html       = true;
	int    maxRuns    = 0;          // --max-runs (0 = test default, not emitted)
	int    maxRecords = 0;          // --max-records (0 = test default, not emitted)
	int    maxRows    = 100000;     // --max-persist-rows (0 = no limit, not emitted)
	String readback   = "sample";   // sample | all
	String pgCleanup  = "keep-last";// keep-last | delete
	// The PostgreSQL connections file (jText, kept OUTSIDE git; "" = the default beside the
	// project: <projects>/SQL_Secrets/jac313_connections.jtext). Only its path is saved here.
	String connFile;
	// The user's own remote PostgreSQL server for "PG remote" (there is no built-in default),
	// loaded from / saved to the connections file. Never a password: libpq reads it from
	// ~/.pgpass or $PGPASSFILE.
	String pgHost;                  // e.g. 10.0.0.5 or db.example.org ("" = not set)
	String pgPort;                  // "" = 5432
	String pgDb       = "jac313";
	String pgUser     = "jac313";

	void Jsonize(JsonIO& io);
};

String          SettingsPath();                 // <exe dir>/Jac313Runner.json
bool            LoadSettings(RunOptions& o);
bool            SaveSettings(const RunOptions& o);
String          FindRootFromExe();              // walk up from the executable to a dir holding a vNNN/CMakeLists.txt

// The summary JSON / CLI flags this GUI understands. The CLI writes "contract": N in every
// summary; a summary with a newer contract is refused (rebuild the GUI). Bump both together.
const int       kRunnerContract = 3;   // 2: --pg-remote / --pg-local, summary "pg_remote"; 3: --connections

// ---- PostgreSQL connections file (jText; see the CLI's connections.hpp for the layout)
struct ConnRow : Moveable<ConnRow> {
	String backend, host, port, dbname, user, timeout, notes;
};
String          DefaultConnectionsPath(const String& root);   // $JAC313_CONNECTIONS, else <root>/../SQL_Secrets/...
String          EffectiveConnFile(const RunOptions& o, const String& root);
bool            LoadConnections(const String& path, Vector<ConnRow>& rows, String& err);
String          ConnectionsText(const Vector<ConnRow>& rows);   // the jText the GUI writes
bool            SaveConnections(const String& path, const Vector<ConnRow>& rows, String& err);
String          EnclosingGitProject(const String& path);        // "" when not inside a git work tree

// libpq conninfo for the Remote PG fields ("" when no host is set).
String          RemoteConninfo(const RunOptions& o);
// jac313_store_dataset under <root>/<version>: the first of the usual build dirs that has it.
String          FindDatasetTool(const String& root, const String& version);

// A version folder under the project root: vNNN/ with a CMakeLists.txt.
struct VersionInfo : Moveable<VersionInfo> {
	String name;       // "v002"
	String standard;   // "C++26" (from cxx_std_NN in its CMake files; "" if not found)
};
Vector<VersionInfo> FindVersions(const String& root);   // sorted oldest -> newest
String          ResolveVersion(const String& wanted, const String& root);   // wanted if present, else newest
String          EffectiveRoot(const RunOptions& o, const String& cmdRoot);

// One child run (the "both" compiler choice makes two).
struct RunPlan {
	String         exe;        // <root>/<version>/jac313_test_cli
	String         cwd;        // <root>/<version>
	String         compiler;   // gcc15 | clang
	String         summary;    // --summary-json path
	Vector<String> args;       // without exe
	String         Display() const;   // exe + args, quoted where needed
};

String          PersistList(const RunOptions& o);   // "sql,jtext,..." ("" = none chosen)
Array<RunPlan>  BuildPlans(const RunOptions& o, const String& root);
// Max persisted in a run can never exceed Max records in a run: with a records limit set,
// a persisted limit of 0 (none) or above it becomes the records limit.
void            LimitPersistToRecords(RunOptions& o);
String          QuoteArg(const String& a);

// ---- Summary JSON ----
struct ResultRow {
	String gate, test, backend, outputMode, status, log;
	String ms, rows;                      // "" when missing
	String rbMode, rbCount, rbChecksum, rbSample, rbChecked;
	String pingMs, writeMs, error;        // readback.ping_ms / write_ms (PostgreSQL), "error"
	String dsBackend, dsBase;
	Vector<String> datasetCmd;            // explicit "dataset_cmd" (preferred when present)
	String compiler, buildType, version;  // copied from the summary that holds the row
	String pgRemote;                      // the summary's remote server (conninfo, no password)
	bool   Failed() const;
};

struct Summary {
	String path, runId, host, version, compiler, buildType, started, finished, command;
	String pgRemote;          // "pg_remote": the remote PostgreSQL server the run used ("" = none)
	int    pass = 0, fail = 0;
	int    contract = 1;      // "contract" (summaries written before it existed are 1)
	Array<ResultRow> rows;
	bool   Load(const String& file, String& err);
	bool   Parse(const String& text, String& err);
};

String          BuildDirFor(const String& compiler, const String& buildType);

// ---- "Copy for AI": one Markdown document someone can paste into any AI and ask
// "what am I looking at?" or "why did this fail?".
struct AiExtra {
	String title;         // the viewer's title for the selected row ("" = nothing selected)
	String datasetText;   // the jText (or log) shown in the viewer
	String validation;    // the viewer's validation line
};
String          AiContextPath(const String& root);   // <root>/gui/ai_context.md (editable project context)
String          BuildAiReport(const Array<Summary>& sums, const String& root, const AiExtra& x);
String          AiReportPath(const Summary& s);      // beside the summary: runner_last*_ai.md
// The dataset command for a row: exe + args ("" exe = no dataset for this row).
bool            DatasetCommand(const ResultRow& r, const String& root, String& exe, Vector<String>& args);

// ---- jText ----
struct JSection {
	String name;
	Vector<String> cols;
	Vector<Vector<String>> rows;   // NULL is stored as JNull()
	int    Width() const;          // max(cols, widest row)
	String ColName(int i) const;   // declared name or "1".."N"
};

struct JDoc {
	Array<JSection> sections;
	Vector<String>  notes;         // tolerated problems (unresolved include, odd lines)
	void Parse(const String& text, const String& baseDir);
	int  MainSection() const;      // first section with rows, else first, else -1
};

const String&   JNull();          // the NULL marker: "\xE2\x88\x85" (empty set sign)

}
