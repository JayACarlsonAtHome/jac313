#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/persistence/SqlEventSink.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

int main() {
    const fs::path out = fs::temp_directory_path() / "jac313_store_sql_smoke";
    fs::remove(out.string() + ".db");
    fs::remove(out.string() + ".sql");

    auto sink = std::make_unique<jac313::Store::v002::SqlEventSink>(
        out.string(), 2, 2, jac313::Store::v002::PersistMode::All, true);
    jac313::Store::v002::DoubleBufferedWriter writer(std::move(sink), 4);

    jac313::Store::v002::PersistedEvent ev{};
    ev.event_id = 1;
    ev.thread_id = 0;
    ev.per_thread_event_id = 0;
    ev.category = "demo";
    ev.payload = "hello";
    ev.int_metrics = {7, 8};
    ev.dbl_metrics = {1.1, 2.2};

    writer.submit_event(std::move(ev));
    writer.finalize();

    if (!fs::exists(out.string() + ".db")) {
        std::cerr << "expected .db file\n";
        return EXIT_FAILURE;
    }

    std::cout << "jac313::Store::v002 SQLite persistence smoke OK\n";
    return EXIT_SUCCESS;
}