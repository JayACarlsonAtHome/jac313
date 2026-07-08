#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/persistence/JsonEventLogReader.hpp>
#include <jac313/Store/v002/headers/persistence/JsonEventSink.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

int main() {
    const fs::path out = fs::temp_directory_path() / "jac313_store_json_smoke";

    for (const auto& suffix : {"", "_Ints", "_Floats"}) {
        fs::remove(out.string() + suffix + ".json");
    }

    auto sink = std::make_unique<jac313::Store::v002::JsonEventSink>(
        out.string(), 2, 2, jac313::Store::v002::PersistMode::All);
    jac313::Store::v002::DoubleBufferedWriter writer(std::move(sink), 4);

    jac313::Store::v002::PersistedEvent ev{};
    ev.event_id = 1;
    ev.thread_id = 0;
    ev.per_thread_event_id = 0;
    ev.category = "demo";
    ev.payload = R"(hello "json")";
    ev.int_metrics = {10, 20};
    ev.dbl_metrics = {1.5, 2.5};

    writer.submit_event(std::move(ev));
    writer.finalize();

    const std::string main_path = out.string() + ".json";
    if (!fs::exists(main_path)) {
        std::cerr << "expected main .json file\n";
        return EXIT_FAILURE;
    }

    jac313::Store::v002::JsonEventLogReader reader(main_path);
    jac313::Store::v002::JsonRecord rec{};
    if (!reader.next(rec)) {
        std::cerr << "reader failed on main record\n";
        return EXIT_FAILURE;
    }
    if (rec.event_id != 1 || rec.category != "demo" || rec.payload != R"(hello "json")") {
        std::cerr << "main record mismatch\n";
        return EXIT_FAILURE;
    }

    jac313::Store::v002::JsonEventLogReader ints_reader(out.string() + "_Ints.json");
    jac313::Store::v002::JsonRecord ints_rec{};
    if (!ints_reader.next(ints_rec)) {
        std::cerr << "reader failed on ints record\n";
        return EXIT_FAILURE;
    }
    if (ints_rec.int_metrics != std::vector<int64_t>{10, 20}) {
        std::cerr << "ints record mismatch\n";
        return EXIT_FAILURE;
    }

    jac313::Store::v002::JsonEventLogReader floats_reader(out.string() + "_Floats.json");
    jac313::Store::v002::JsonRecord floats_rec{};
    if (!floats_reader.next(floats_rec)) {
        std::cerr << "reader failed on floats record\n";
        return EXIT_FAILURE;
    }
    if (floats_rec.dbl_metrics != std::vector<double>{1.5, 2.5}) {
        std::cerr << "floats record mismatch\n";
        return EXIT_FAILURE;
    }

    std::cout << "jac313::Store::v002 JSON persistence smoke OK\n";
    return EXIT_SUCCESS;
}