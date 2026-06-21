#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventLogReader.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventSink.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

int main() {
    const fs::path out = fs::temp_directory_path() / "jac313_store_binary_smoke";
    fs::remove(out.string() + ".bin");

    auto sink = std::make_unique<jac313::Store::v002::BinaryEventSink>(
        out.string(), 2, 2, jac313::Store::v002::PersistMode::All, 1 << 20);
    jac313::Store::v002::DoubleBufferedWriter writer(std::move(sink), 4);

    jac313::Store::v002::PersistedEvent ev{};
    ev.event_id = 1;
    ev.thread_id = 0;
    ev.per_thread_event_id = 0;
    ev.category = "demo";
    ev.payload = "hello";
    ev.int_metrics = {1, 2};
    ev.dbl_metrics = {1.0, 2.0};

    writer.submit_event(std::move(ev));
    writer.finalize();

    const std::string bin_path = out.string() + ".bin";
    if (!fs::exists(bin_path)) {
        std::cerr << "expected .bin file\n";
        return EXIT_FAILURE;
    }

    jac313::Store::v002::BinaryEventLogReader reader(bin_path);
    jac313::Store::v002::BinaryRecord rec{};
    if (!reader.next(rec)) {
        std::cerr << "expected one record in .bin file\n";
        return EXIT_FAILURE;
    }
    if (rec.event_id != 1 || rec.payload != "hello" || rec.int_metrics != std::vector<int64_t>{1, 2}) {
        std::cerr << "read-back mismatch\n";
        return EXIT_FAILURE;
    }

    std::cout << "jac313::Store::v002 binary persistence smoke OK\n";
    return EXIT_SUCCESS;
}