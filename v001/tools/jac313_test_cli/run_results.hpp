#pragma once

// The durable run results: results.db's run_result (the main table) and run_change (the notes) are kept
// in two tracked jText files beside results.db, test-summary/results_main.jtext and results_note.jtext,
// one table each, newest first. results.db is local and can be wiped; the files are the record.
//
//   load     every time the CLI opens results.db to record or report: the files replace both tables
//            (results.db is never the record). With no results_main.jtext yet, the tables are made
//            from what results.db holds and saved to the files.
//   save     after every recorded run: both tables to their files.
//   publish  at the end of the run: commit only the two files on the current branch, and push that
//            branch with --no-verify when it is dev-work (no other branch is pushed).
//            $JAC313_RESULTS_PUBLISH = defer | off (as the CLI started) saves without committing:
//            the Runner defers and publishes once at its end (results-publish); the pre-push gate
//            sets off, and its rows go with the next commit.
//   announce before a run starts: has this commit passed or failed here before?

#include <jac313/Qlite/v001.hpp>

#include <cstdint>
#include <filesystem>
#include <string>

namespace jac313::test_cli::run_results {

using Sqlite = jac313::Qlite::v001::Sqlite;

// The checkout the version folder is in: its HEAD commit, and 1 when the version folder has uncommitted
// changes (then the commit alone does not name the code). ("", 0) outside a git checkout.
struct GitState {
    std::string commit;
    std::int64_t dirty = 0;
};
GitState git_state_of(const std::filesystem::path& version_dir);

std::filesystem::path main_file();   // <results.db folder>/results_main.jtext
std::filesystem::path note_file();   // <results.db folder>/results_note.jtext

void load(Sqlite& db);
void save(Sqlite& db);

// Record the run_result row of a run just recorded, save both files, and remember the run for publish.
void record(Sqlite& db, std::int64_t run_id, const std::string& gate, const std::filesystem::path& version_dir);

// True when this process recorded a run, or asked to publish (run-everything).
bool pending();
void request_publish();

// Commit and push as described above. force: publish even when $JAC313_RESULTS_PUBLISH said defer
// (the results-publish command). 0 = done or nothing to do; a failure is reported, never fatal.
int publish(bool force);

// Print what results_main.jtext says about this checkout's commit before a run starts: each earlier run of
// it in this version (pass or fail, the gate, the compiler, the machine), or that it has none. Once per
// run: a CLI started by another (run-everything's steps) stays quiet.
void announce(const std::filesystem::path& version_dir, const std::string& version);

}  // namespace jac313::test_cli::run_results
