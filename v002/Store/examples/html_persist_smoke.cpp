#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/persistence/HtmlEventSink.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

int main() {
    const fs::path out = fs::temp_directory_path() / "jac313_store_html_smoke";

    for (const auto& suffix : {"", "_Ints", "_Floats"}) {
        fs::remove(out.string() + suffix + ".html");
    }

    auto sink = std::make_unique<jac313::Store::v002::HtmlEventSink>(
        out.string(), 2, 2, jac313::Store::v002::PersistMode::All);
    jac313::Store::v002::DoubleBufferedWriter writer(std::move(sink), 4);

    jac313::Store::v002::PersistedEvent ev{};
    ev.event_id = 1;
    ev.thread_id = 0;
    ev.per_thread_event_id = 0;
    ev.category = "demo";
    ev.payload = "hello <world>";
    ev.int_metrics = {10, 20};
    ev.dbl_metrics = {1.5, 2.5};

    writer.submit_event(std::move(ev));
    writer.finalize();

    if (!fs::exists(out.string() + ".html")) {
        std::cerr << "expected main .html file\n";
        return EXIT_FAILURE;
    }

    std::cout << "jac313::Store::v002 HTML persistence smoke OK\n";
    return EXIT_SUCCESS;
}