#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventLogReader.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/FlagRoutingEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/SqlEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/Sqlite.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

namespace {
constexpr uint64_t kKeeperFlag   = 1ULL << 1;
constexpr uint64_t kDatabaseFlag = 1ULL << 2;

jac313::Store::v002::PersistedEvent make_event(
    size_t id, uint64_t flags, std::string_view payload)
{
    jac313::Store::v002::PersistedEvent ev{};
    ev.event_id = id;
    ev.thread_id = 0;
    ev.per_thread_event_id = id;
    ev.flags = flags;
    ev.category = "routing";
    ev.payload = payload;
    ev.int_metrics = {static_cast<int64_t>(id)};
    ev.dbl_metrics = {static_cast<double>(id)};
    return ev;
}
} // namespace

int main() {
    const fs::path out = fs::temp_directory_path() / "jac313_store_flag_routing_smoke";
    fs::remove(out.string() + ".bin");
    fs::remove(out.string() + ".db");
    fs::remove(out.string() + ".sql");

    auto file_sink = std::make_unique<jac313::Store::v002::BinaryEventSink>(
        out.string(), 1, 1, jac313::Store::v002::PersistMode::KeeperOnly, 1 << 20);
    auto sql_sink = std::make_unique<jac313::Store::v002::SqlEventSink>(
        out.string(), 1, 1, jac313::Store::v002::PersistMode::DatabaseOnly, false);

    auto routing_sink = std::make_unique<jac313::Store::v002::FlagRoutingEventSink>(
        std::move(file_sink), std::move(sql_sink));
    jac313::Store::v002::DoubleBufferedWriter writer(std::move(routing_sink), 4);

    writer.submit_event(make_event(1, kKeeperFlag, "keeper-only"));
    writer.submit_event(make_event(2, kDatabaseFlag, "database-only"));
    writer.submit_event(make_event(3, kKeeperFlag | kDatabaseFlag, "both"));
    writer.finalize();

    if (!fs::exists(out.string() + ".bin")) {
        std::cerr << "expected .bin file from KeeperRecord routing\n";
        return EXIT_FAILURE;
    }
    const std::string bin_path = out.string() + ".bin";
    const std::string db_path  = out.string() + ".db";
    if (!fs::exists(db_path)) {
        std::cerr << "expected .db file from DatabaseEntry routing\n";
        return EXIT_FAILURE;
    }

    jac313::Store::v002::BinaryEventLogReader reader(bin_path);
    jac313::Store::v002::BinaryRecord rec{};
    std::vector<uint64_t> keeper_ids;
    while (reader.next(rec)) {
        keeper_ids.push_back(rec.event_id);
    }
    if (keeper_ids != std::vector<uint64_t>{1, 3}) {
        std::cerr << "expected keeper records for events 1 and 3\n";
        return EXIT_FAILURE;
    }

    jac313::Store::v002::Sqlite db(db_path);
    auto count_stmt = db.prepare(
        "SELECT COUNT(*) FROM jac313_store_flag_routing_smoke");
    int64_t sql_rows = 0;
    if (count_stmt.step()) {
        count_stmt.get(sql_rows);
    }
    if (sql_rows != 2) {
        std::cerr << "expected 2 SQL rows for events 2 and 3\n";
        return EXIT_FAILURE;
    }

    std::cout << "jac313::Store::v002 FlagRouting persistence smoke OK\n";
    return EXIT_SUCCESS;
}