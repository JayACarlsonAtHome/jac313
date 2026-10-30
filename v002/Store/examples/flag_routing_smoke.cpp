// Flag routing without a router: two sinks attached to one store, one worker each. The binary log
// takes PersistMode::KeeperOnly (KeeperRecord, bit 1), the SQLite sink PersistMode::DatabaseOnly
// (DatabaseEntry, bit 2); both walk the same rows and each keeps only the events its mode takes.

#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventLogReader.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/SqlEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/Sqlite.hpp>

#include <array>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;
namespace S = jac313::Store::v002;

namespace {
constexpr uint64_t kKeeperFlag   = 1ULL << 1;
constexpr uint64_t kDatabaseFlag = 1ULL << 2;

using Config = S::ts_store_config<true, 6, 20, 43, 1, 1, false>;
using Store  = S::ts_store<Config>;
} // namespace

int main() {
    const fs::path out = fs::temp_directory_path() / "jac313_store_flag_routing_smoke";
    const std::string bin_path = out.string() + ".bin";
    const std::string db_path  = out.string() + ".db";
    fs::remove(bin_path);
    fs::remove(db_path);
    fs::remove(out.string() + ".sql");

    // Event ids 0, 1, 2: keeper only, database only, both.
    const std::array<uint64_t, 3> flags{kKeeperFlag, kDatabaseFlag, kKeeperFlag | kDatabaseFlag};
    {
        Store store(1, flags.size());
        const size_t bin = store.attach_persistence(
            std::make_unique<S::BinaryEventSink>(out.string(), 1, 1, S::PersistMode::KeeperOnly, 1 << 20), 1);
        const size_t sql = store.attach_persistence(
            std::make_unique<S::SqlEventSink>(out.string(), 1, 1, S::PersistMode::DatabaseOnly, false), 1);
        for (size_t i = 0; i < flags.size(); ++i) {
            (void)store.save_event(0, i, std::string("routing-") + std::to_string(i), flags[i],
                                   std::string("routing"), false,
                                   {static_cast<int64_t>(i)}, {static_cast<double>(i)});
        }
        store.finalize_persistence();

        for (const size_t w : {bin, sql}) {
            if (store.persistence_failed(w) || store.persisted_rows(w) != flags.size()) {
                std::cerr << "sink " << store.persistence_sink_name(w) << " did not take all "
                          << flags.size() << " rows\n";
                return EXIT_FAILURE;
            }
            std::cout << "  " << store.persistence_sink_name(w) << ": " << store.persisted_rows(w)
                      << " rows walked, drained in " << store.drain_time_us(w) << " us\n";
        }
    }

    if (!fs::exists(bin_path)) {
        std::cerr << "expected .bin file from KeeperRecord routing\n";
        return EXIT_FAILURE;
    }
    if (!fs::exists(db_path)) {
        std::cerr << "expected .db file from DatabaseEntry routing\n";
        return EXIT_FAILURE;
    }

    S::BinaryEventLogReader reader(bin_path);
    S::BinaryRecord rec{};
    std::vector<uint64_t> keeper_ids;
    while (reader.next(rec)) {
        keeper_ids.push_back(rec.event_id);
    }
    if (keeper_ids != std::vector<uint64_t>{0, 2}) {
        std::cerr << "expected keeper records for events 0 and 2\n";
        return EXIT_FAILURE;
    }

    S::Sqlite db(db_path);
    auto ids_stmt = db.prepare(
        "SELECT id FROM jac313_store_flag_routing_smoke ORDER BY id");
    std::vector<int64_t> sql_ids;
    while (ids_stmt.step()) {
        int64_t id = 0;
        ids_stmt.get(id);
        sql_ids.push_back(id);
    }
    if (sql_ids != std::vector<int64_t>{1, 2}) {
        std::cerr << "expected SQL rows for events 1 and 2\n";
        return EXIT_FAILURE;
    }

    std::cout << "jac313::Store::v002 flag routing (two sinks, one worker each) smoke OK\n";
    return EXIT_SUCCESS;
}
