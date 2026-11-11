#include "run_results.hpp"

#include "process.hpp"
#include "settings.hpp"
#include "jac313_results_db.hpp"

#include <jac313/jText/v001.hpp>
#include <sqlite3.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace jac313::test_cli::run_results {

namespace fs = std::filesystem;
namespace R = jac313::results;

namespace {

// $JAC313_RESULTS_PUBLISH as this process started (run-everything changes it for its steps).
const std::string g_publish_mode = [] {
    const char* m = std::getenv("JAC313_RESULTS_PUBLISH");
    return std::string(m ? m : "");
}();
std::vector<std::string> g_recorded;   // "v001 run 71 ctest clang21 pass", for the commit message
bool g_requested = false;

std::string chomp(std::string s) {
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r' || s.back() == ' ')) s.pop_back();
    return s;
}

// git -C dir args...: its output, and its exit code in *code.
std::string git(const fs::path& dir, std::vector<std::string> args, int* code = nullptr, int timeout_sec = 60) {
    args.insert(args.begin(), {"git", "-C", dir.string()});
    const ProcessResult r = run_process(args, timeout_sec);
    if (code) *code = r.started ? r.exit_code : -1;
    return chomp(r.stdout_text + (r.exit_code != 0 ? r.stderr_text : std::string{}));
}

// A value as one jText field: no line breaks or tabs.
std::string flat(std::string s) {
    for (char& c : s)
        if (c == '\n' || c == '\r' || c == '\t') c = ' ';
    return s;
}

struct Column { std::string name, type; };
std::vector<Column> columns_of(Sqlite& db, const std::string& table) {
    std::vector<Column> cols;
    auto st = db.prepare("SELECT name, type FROM pragma_table_info('" + table + "') ORDER BY cid");
    while (st.step()) {
        Column c;
        st.get(c.name, c.type);
        cols.push_back(std::move(c));
    }
    return cols;
}

// The text of column i of the current row ('' for NULL, which this schema never stores).
std::string text_at(sqlite3_stmt* st, int i) {
    const unsigned char* t = sqlite3_column_text(st, i);
    return t ? std::string(reinterpret_cast<const char*>(t)) : std::string{};
}

// One table to its own jText file, newest first. Written beside the file and renamed over it, so a
// reader never sees half a file.
void write_table(Sqlite& db, const std::string& table, const std::string& order, const fs::path& file,
                 const std::string& purpose) {
    using namespace jac313::jText::v001;
    const std::vector<Column> cols = columns_of(db, table);
    if (cols.empty()) throw std::runtime_error("results.db has no table " + table);
    const fs::path tmp = file.string() + ".tmp";
    {
        std::ofstream out(tmp, std::ios::trunc);
        if (!out) throw std::runtime_error("cannot write " + tmp.string());
        JTextWriter w(out);
        w.set_profile(JTextProfile::Full);
        w.set_filename(file.filename().string());
        w.set_purpose(purpose);
        w.add_header_comment_line(0, "Written by jac313_test_cli from results.db table " + table
                                     + " after every recorded run, newest first, and loaded back into it on");
        w.add_header_comment_line(0, "every start. Commit it with the work: results.db is local, this file is the record.");
        w.write_header();
        w.begin_section(table);
        w.begin_fields();
        for (std::size_t i = 0; i < cols.size(); ++i) {
            JTextEntry e;
            e.number = i + 1;
            e.delimiter = U'#';
            e.level_sep = U'/';
            const bool integer = cols[i].type.find("INT") != std::string::npos;
            e.fields = {cols[i].name, integer ? "Integer" : "String", "Not Null"};
            w.append_entry(e);
        }
        w.begin_data();
        std::string sql = "SELECT ";
        for (std::size_t i = 0; i < cols.size(); ++i) sql += (i ? ", " : "") + cols[i].name;
        sql += " FROM " + table + " ORDER BY " + order;
        auto st = db.prepare(sql);
        while (st.step()) {
            std::vector<std::string> row;
            for (int i = 0; i < static_cast<int>(cols.size()); ++i) row.push_back(flat(text_at(st.handle(), i)));
            w.append_row(1, row);
            w.end_record();
        }
        w.finalize();
    }
    std::error_code ec;
    fs::rename(tmp, file, ec);
    if (ec) throw std::runtime_error("cannot replace " + file.string() + ": " + ec.message());
}

// One table's rows back from its file, newest first, keeping every id. Columns the file has
// and the table does not are left out; columns the table has and the file does not take their default.
void read_table(Sqlite& db, const std::string& table, const fs::path& file) {
    const SettingsTable t = read_jtext_table(file, table);
    std::vector<std::string> use;
    for (const Column& c : columns_of(db, table))
        if (std::find(t.columns.begin(), t.columns.end(), c.name) != t.columns.end()) use.push_back(c.name);
    if (use.empty()) return;
    std::vector<const std::map<std::string, std::string>*> rows;
    for (const auto& r : t.rows) rows.push_back(&r);
    // newest first: by when the run started (run_result), or when the note was written (run_change)
    const std::string key = std::find(t.columns.begin(), t.columns.end(), "run_started") != t.columns.end() ? "run_started" : "ts_utc";
    std::stable_sort(rows.begin(), rows.end(), [&key](const auto* a, const auto* b) {
        const auto ta = a->find(key), tb = b->find(key);
        return (ta == a->end() ? std::string{} : ta->second) > (tb == b->end() ? std::string{} : tb->second);   // newest first
    });
    std::string sql = "INSERT INTO " + table + "(";
    std::string marks;
    for (std::size_t i = 0; i < use.size(); ++i) {
        sql += (i ? ", " : "") + use[i];
        marks += i ? ", ?" : "?";
    }
    sql += ") VALUES(" + marks + ")";
    auto st = db.prepare(sql);
    for (const auto* r : rows) {
        st.reset();
        st.clear_bindings();
        for (std::size_t i = 0; i < use.size(); ++i) st.bind_text(static_cast<int>(i + 1), r->at(use[i]));
        st.step();
    }
}

// Every run results.db holds, as run_result rows (the commit is not known for a run recorded before
// run_result: '').
void make_from_db(Sqlite& db) {
    std::vector<std::int64_t> runs;
    { auto st = db.prepare("SELECT run_id FROM run ORDER BY run_id");
      while (st.step()) { std::int64_t id = 0; st.get(id); runs.push_back(id); } }
    for (const std::int64_t id : runs) R::record_run_result(db, id, "", "", 0);
}

}  // namespace

GitState git_state_of(const fs::path& version_dir) {
    const fs::path dir = version_dir.empty() ? fs::current_path() : version_dir;
    int code = 0;
    const std::string commit = git(dir, {"rev-parse", "HEAD"}, &code);
    if (code != 0) return {};
    const std::string status = git(dir, {"-c", "core.fileMode=false", "status", "--porcelain", "--", "."}, &code);
    return {commit, (code != 0 || !status.empty()) ? std::int64_t{1} : std::int64_t{0}};
}

fs::path main_file() { return fs::path(R::JAC313_RESULTS_DB).parent_path() / "results_main.jtext"; }
fs::path note_file() { return fs::path(R::JAC313_RESULTS_DB).parent_path() / "results_note.jtext"; }

void load(Sqlite& db) {
    R::ensure_schema(db);
    std::error_code ec;
    const bool have_main = fs::exists(main_file(), ec);
    auto tx = db.transaction();
    db.exec("DROP TABLE IF EXISTS run_change");
    db.exec("DROP TABLE IF EXISTS run_result");
    R::detail::create_results_tables(db);
    if (have_main) {
        read_table(db, "run_result", main_file());
        if (fs::exists(note_file(), ec)) read_table(db, "run_change", note_file());
    } else {
        make_from_db(db);
    }
    tx.commit();
    if (!have_main) {
        save(db);
        std::cout << "[results] started " << main_file().string() << " and " << note_file().filename().string()
                  << " from results.db\n";
    }
}

void save(Sqlite& db) {
    write_table(db, "run_result", "run_started DESC, id DESC", main_file(), "Run results: one row per recorded run (results.db run_result)");
    write_table(db, "run_change", "ts_utc DESC, id DESC", note_file(), "Run notes: what we changed for a run (results.db run_change)");
}

void record(Sqlite& db, std::int64_t run_id, const std::string& gate, const fs::path& version_dir) {
    const GitState g = git_state_of(version_dir);
    R::record_run_result(db, run_id, gate, g.commit, g.dirty);
    save(db);
    std::string version, gates, compiler, result;
    { auto st = db.prepare("SELECT version, gate, compiler, result FROM run_result WHERE run_id=? ORDER BY ts_utc DESC LIMIT 1");
      st.bind(run_id); if (st.step()) st.get(version, gates, compiler, result); }
    g_recorded.push_back(version + " run " + std::to_string(run_id) + " " + gates + " " + compiler + " " + result);
}

bool pending() { return g_requested || !g_recorded.empty(); }
void request_publish() { g_requested = true; }

namespace {

// The key of a run_result row: a run, wherever and whenever it ran (the table's UNIQUE key).
constexpr const char* kRunKey = "r.pin = k.pin AND r.version = k.version AND r.run_id = k.run_id AND r.run_started = k.run_started";

// Put back the rows this machine has and the pulled files do not (kept in keep_result / keep_change
// before the pull): each gets the next id, and its notes follow it.
void merge_kept(Sqlite& db) {
    std::string cols;
    for (const Column& c : columns_of(db, "run_result"))
        if (c.name != "id") cols += (cols.empty() ? "" : ", ") + c.name;
    std::vector<std::pair<std::int64_t, std::int64_t>> moved;   // kept id -> its id now
    {
        auto st = db.prepare(std::string("SELECT k.id, COALESCE((SELECT r.id FROM run_result r WHERE ") + kRunKey
                             + "), 0) FROM temp.keep_result k ORDER BY k.run_started, k.id");
        while (st.step()) {
            std::int64_t kept = 0, now = 0;
            st.get(kept, now);
            moved.emplace_back(kept, now);
        }
    }
    for (auto& [kept, now] : moved) {
        if (now != 0) continue;   // the pulled file has this run already
        now = R::get_top_one_long(db, "SELECT COALESCE(MAX(id), 0) + 1 FROM run_result");
        db.exec("INSERT INTO run_result(id, " + cols + ") SELECT ?, " + cols + " FROM temp.keep_result WHERE id = ?", now, kept);
    }
    for (const auto& [kept, now] : moved)
        db.exec("INSERT INTO run_change(run_result_id, ts_utc, change_text) SELECT ?, c.ts_utc, c.change_text "
                "FROM temp.keep_change c WHERE c.run_result_id = ? AND NOT EXISTS (SELECT 1 FROM run_change n "
                "WHERE n.run_result_id = ? AND n.ts_utc = c.ts_utc AND n.change_text = c.change_text)",
                now, kept, now);
}

// Unpushed commits on HEAD that change anything but the two results files ('' = none).
std::string unpushed_code(const fs::path& dir, const std::string& m, const std::string& n) {
    int code = 0;
    const std::string files = git(dir, {"diff", "--name-only", "origin/dev-work", "HEAD"}, &code);
    if (code != 0) return "cannot compare with origin/dev-work";
    const fs::path top = git(dir, {"rev-parse", "--show-toplevel"});
    std::string other;
    std::size_t start = 0;
    while (start < files.size()) {
        std::size_t end = files.find('\n', start);
        if (end == std::string::npos) end = files.size();
        const std::string f = files.substr(start, end - start);
        if (!f.empty() && (top / f).string() != m && (top / f).string() != n) other += (other.empty() ? "" : ", ") + f;
        start = end + 1;
    }
    return other;
}

}  // namespace

int publish(bool force) {
    if (!force && (g_publish_mode == "defer" || g_publish_mode == "off")) {
        if (!g_recorded.empty())
            std::cout << "[results] saved to " << main_file().filename().string() << " (commit " << g_publish_mode
                      << ": $JAC313_RESULTS_PUBLISH)\n";
        return 0;
    }
    const fs::path dir = main_file().parent_path();
    int code = 0;
    git(dir, {"rev-parse", "--show-toplevel"}, &code);
    if (code != 0) { std::cout << "[results] not committed: " << dir.string() << " is not in a git checkout\n"; return 0; }
    const std::string m = main_file().string(), n = note_file().string();
    const std::string branch = git(dir, {"rev-parse", "--abbrev-ref", "HEAD"});
    const bool on_dev_work = branch == "dev-work";
    try {
        Sqlite db(R::JAC313_RESULTS_DB);
        R::ensure_schema(db);
        if (on_dev_work) {
            // 1. pull: the files as dev-work has them now. This machine's rows wait in temp tables
            //    (results.db has them all), and the files' own edits are set aside so the pull can apply.
            db.exec("DROP TABLE IF EXISTS temp.keep_result");
            db.exec("DROP TABLE IF EXISTS temp.keep_change");
            db.exec("CREATE TEMP TABLE keep_result AS SELECT * FROM main.run_result");
            db.exec("CREATE TEMP TABLE keep_change AS SELECT * FROM main.run_change");
            git(dir, {"checkout", "--", m, n}, &code);   // a file not committed yet has nothing to restore
            const std::string pulled = git(dir, {"pull", "--ff-only", "--no-rebase", "origin", "dev-work"}, &code, 180);
            if (code != 0)
                std::cout << "[results] pull FAILED (going on with this machine's copy):\n" << pulled << '\n';
            // 2. load the pulled files, 3. put this machine's new rows on top
            load(db);
            auto tx = db.transaction();
            merge_kept(db);
            tx.commit();
            db.exec("DROP TABLE IF EXISTS temp.keep_result");
            db.exec("DROP TABLE IF EXISTS temp.keep_change");
        }
        // 4. export each table to its file
        save(db);
    } catch (const std::exception& e) {
        std::cout << "[results] update FAILED: " << e.what() << '\n';
        return 1;
    }
    // 5. commit only the two files
    if (git(dir, {"status", "--porcelain", "--", m, n}, &code).empty() && code == 0) {
        std::cout << "[results] " << main_file().filename().string() << " and " << note_file().filename().string()
                  << " unchanged: nothing to commit\n";
        return 0;
    }
    std::string msg = "Run results";
    if (!g_recorded.empty()) {
        msg += ": ";
        for (std::size_t i = 0; i < g_recorded.size(); ++i) msg += (i ? ", " : "") + g_recorded[i];
    }
    git(dir, {"add", "--", m, n}, &code);
    const std::string out = git(dir, {"commit", "--no-verify", "-m", msg, "--", m, n}, &code);
    if (code != 0) {
        std::cout << "[results] commit FAILED (the files are saved; commit them by hand):\n" << out << '\n';
        return 1;
    }
    std::cout << "[results] committed " << main_file().filename().string() << " + " << note_file().filename().string()
              << " on " << branch << ": " << msg << '\n';
    // 6. push dev-work, and only when nothing but run results would go with it (code goes through the gate)
    if (!on_dev_work) {
        std::cout << "[results] not pushed: run results are pushed only from dev-work (this is " << branch << ")\n";
        return 0;
    }
    if (const std::string code_files = unpushed_code(dir, m, n); !code_files.empty()) {
        std::cout << "[results] not pushed: dev-work has unpushed code (" << code_files
                  << "); push it through the gate and the results go with it\n";
        return 0;
    }
    const std::string pushed = git(dir, {"push", "--no-verify", "origin", "dev-work"}, &code, 180);
    if (code != 0) {
        std::cout << "[results] push FAILED (committed; the next run pulls and pushes again):\n" << pushed << '\n';
        return 1;
    }
    std::cout << "[results] pushed dev-work (--no-verify)\n";
    return 0;
}

void announce(const fs::path& version_dir, const std::string& version) {
    if (std::getenv("JAC313_RESULTS_ANNOUNCED")) return;
    ::setenv("JAC313_RESULTS_ANNOUNCED", "1", 1);   // the steps this run starts stay quiet
    const GitState g = git_state_of(version_dir);
    if (g.commit.empty()) { std::cout << "[results] this is not a git checkout: no earlier results to compare\n"; return; }
    std::cout << "[results] commit " << g.commit.substr(0, 12)
              << (g.dirty ? " + uncommitted changes in " + version + " (earlier results of the commit do not cover them)" : std::string{})
              << '\n';
    try {
        Sqlite db(R::JAC313_RESULTS_DB);
        load(db);
        auto st = db.prepare("SELECT run_id, ts_utc, gate, compiler, build_type, modules, result, pass_count, fail_count, "
                             "git_dirty, pin, os_name FROM run_result WHERE git_commit=? AND version=? ORDER BY ts_utc DESC");
        st.bind(g.commit, version);
        int passed = 0, failed = 0;
        std::vector<std::string> lines;
        while (st.step()) {
            std::int64_t run = 0, pass = 0, fail = 0, dirty = 0;
            std::string ts, gate, compiler, build_type, modules, result, pin, os;
            st.get(run, ts, gate, compiler, build_type, modules, result, pass, fail, dirty, pin, os);
            (result == "pass" ? passed : failed)++;
            lines.push_back("  " + std::string(result == "pass" ? "PASS" : "FAIL") + "  run " + std::to_string(run) + "  " + ts
                            + "  " + gate + "  " + compiler + "  " + build_type + (modules == "on" ? " modules" : "")
                            + "  pass " + std::to_string(pass) + " fail " + std::to_string(fail) + "  " + pin + " " + os
                            + (dirty ? "  (with uncommitted changes)" : ""));
        }
        if (lines.empty()) {
            std::cout << "[results] this commit has no earlier run in " << version << " (" << main_file().filename().string() << ")\n";
            return;
        }
        std::cout << "[results] this commit ran before in " << version << ": " << passed << " passed, " << failed
                  << " failed" << (failed ? "  <-- FAILED BEFORE" : "") << '\n';
        for (const auto& l : lines) std::cout << l << '\n';
    } catch (const std::exception& e) {
        std::cout << "[results] cannot read the earlier results: " << e.what() << '\n';
    }
}

}  // namespace jac313::test_cli::run_results
