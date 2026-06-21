#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventLogReader.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventSink.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

int main() {
    const fs::path out = fs::temp_directory_path() / "jac313_store_binary_to_jtext_smoke";
    fs::remove(out.string() + ".bin");
    for (const auto& suffix : {"", "_Ints", "_Floats"}) {
        fs::remove(out.string() + suffix + ".jtext");
    }

    auto sink = std::make_unique<jac313::Store::v002::BinaryEventSink>(
        out.string(), 2, 2, jac313::Store::v002::PersistMode::All, 1 << 20);
    jac313::Store::v002::DoubleBufferedWriter writer(std::move(sink), 4);

    jac313::Store::v002::PersistedEvent ev{};
    ev.event_id = 42;
    ev.thread_id = 0;
    ev.per_thread_event_id = 0;
    ev.category = "convert";
    ev.payload = "binary-to-jtext";
    ev.int_metrics = {7, 8};
    ev.dbl_metrics = {1.1, 2.2};

    writer.submit_event(std::move(ev));
    writer.finalize();

    const std::string bin_path = out.string() + ".bin";
    jac313::Store::v002::BinaryEventLogReader reader(bin_path);
    reader.convert_to_jtext(out.string(), 2, 2);

    if (!fs::exists(out.string() + ".jtext")) {
        std::cerr << "expected converted main .jtext file\n";
        return EXIT_FAILURE;
    }

    std::cout << "jac313::Store::v002 binary-to-jtext smoke OK\n";
    return EXIT_SUCCESS;
}