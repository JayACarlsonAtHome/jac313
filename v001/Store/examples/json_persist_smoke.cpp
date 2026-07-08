#include <jac313/Store/v001.hpp>
#include <jac313/Store/v001/headers/persistence/JsonEventSink.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

int main() {
    const fs::path out = fs::temp_directory_path() / "jac313_store_json_smoke";

    for (const auto& suffix : {"", "_Ints", "_Floats"}) {
        fs::remove(out.string() + suffix + ".json");
    }

    auto sink = std::make_unique<jac313::Store::v001::JsonEventSink>(
        out.string(), 2, 2, jac313::Store::v001::PersistMode::All);
    jac313::Store::v001::DoubleBufferedWriter writer(std::move(sink), 4);

    jac313::Store::v001::PersistedEvent ev{};
    ev.event_id = 1;
    ev.thread_id = 0;
    ev.per_thread_event_id = 0;
    ev.category = "demo";
    ev.payload = R"(hello "json")";
    ev.int_metrics = {10, 20};
    ev.dbl_metrics = {1.5, 2.5};

    writer.submit_event(std::move(ev));
    writer.finalize();

    if (!fs::exists(out.string() + ".json")) {
        std::cerr << "expected main .json file\n";
        return EXIT_FAILURE;
    }
    if (!fs::exists(out.string() + "_Ints.json")) {
        std::cerr << "expected _Ints .json file\n";
        return EXIT_FAILURE;
    }
    if (!fs::exists(out.string() + "_Floats.json")) {
        std::cerr << "expected _Floats .json file\n";
        return EXIT_FAILURE;
    }

    std::cout << "jac313::Store::v001 JSON persistence smoke OK\n";
    return EXIT_SUCCESS;
}