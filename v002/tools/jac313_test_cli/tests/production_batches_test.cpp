// The production bench run's per-sink batches (results::production_batches): each sink at this
// machine's io_best_fit.useThis for its type, the fallback for a type not calibrated, another
// machine's rows never used, and a --batch override giving all three the same value.

#include "jac313_results_db.hpp"

#include <jac313/Qlite/v002.hpp>

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <source_location>
#include <string>
#include <string_view>
#include <unistd.h>

using Sqlite = jac313::Qlite::v002::Sqlite;
namespace fs = std::filesystem;
namespace R = jac313::results;

namespace {

int failures = 0;

void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    ++failures;
}

bool same(const R::SinkBatches& b, std::int64_t binary, std::int64_t keeper, std::int64_t database) {
    return b.binary == binary && b.keeper == keeper && b.database == database;
}

} // namespace

int main() {
    const fs::path path = fs::temp_directory_path()
                        / ("jac313_production_batches_test_" + std::to_string(::getpid()) + ".db");
    fs::remove(path);
    {
        Sqlite db(path.string());
        R::ensure_schema(db);
        const std::int64_t mid = 7, other = 8;
        R::upsert_io_best_fit(db, mid, "binary", 3000, 5000, 4500, 100, 4000);
        R::upsert_io_best_fit(db, mid, "jtext", 2000, 4000, 3500, 100, 3000);
        R::upsert_io_best_fit(db, mid, "sql", 1000, 3000, 2500, 100, 2000);
        R::upsert_io_best_fit(db, other, "html", 100, 900, 500, 100, 700);

        check(same(R::production_batches(db, mid, "jtext", "sql", 10000, 0), 4000, 3000, 2000),
              "each sink at its type's useThis");
        check(same(R::production_batches(db, mid, "html", "pg-remote", 10000, 0), 4000, 10000, 10000),
              "an uncalibrated type falls back (another machine's html row is not used)");
        check(same(R::production_batches(db, other, "jtext", "sql", 10000, 0), 10000, 10000, 10000),
              "a machine without rows: every sink at the fallback");
        check(same(R::production_batches(db, mid, "jtext", "sql", 10000, 500), 500, 500, 500),
              "--batch overrides all three");
        check(same(R::production_batches(db, 0, "jtext", "sql", 10000, 0), 10000, 10000, 10000),
              "no host pin (machine 0): the fallback");
    }
    fs::remove(path);
    if (failures) { std::cerr << failures << " check(s) failed\n"; return 1; }
    std::cout << "production_batches_test: OK\n";
    return 0;
}
