#include <jac313/Store/v001.hpp>
#include <jac313/Store/v001/headers/persistence/BinaryEventLogReader.hpp>
#include <jac313/Store/v001/headers/persistence/BinaryEventSink.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

#define CHECK(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
            return EXIT_FAILURE; \
        } \
    } while (0)

int main() {
    const fs::path out = fs::temp_directory_path() / "jac313_store_binary_reader_test";
    fs::remove(out.string() + ".bin");
    for (const auto& suffix : {"", "_Ints", "_Floats"}) {
        fs::remove(out.string() + suffix + ".jtext");
    }

    auto sink = std::make_unique<jac313::Store::v001::BinaryEventSink>(
        out.string(), 2, 2, jac313::Store::v001::PersistMode::All, 1 << 20);
    jac313::Store::v001::DoubleBufferedWriter writer(std::move(sink), 4);

    jac313::Store::v001::PersistedEvent ev{};
    ev.event_id = 99;
    ev.thread_id = 1;
    ev.per_thread_event_id = 7;
    ev.category = "reader-test";
    ev.payload = "round-trip";
    ev.int_metrics = {11, 22};
    ev.dbl_metrics = {3.3, 4.4};

    writer.submit_event(std::move(ev));
    writer.finalize();

    const std::string bin_path = out.string() + ".bin";
    CHECK(fs::exists(bin_path));

    jac313::Store::v001::BinaryEventLogReader reader(bin_path);
    jac313::Store::v001::BinaryRecord rec{};
    CHECK(reader.next(rec));
    CHECK(rec.event_id == 99);
    CHECK(rec.thread_id == 1);
    CHECK(rec.per_thread_event_id == 7);
    CHECK(rec.category == "reader-test");
    CHECK(rec.payload == "round-trip");
    CHECK((rec.int_metrics == std::vector<int64_t>{11, 22}));
    CHECK((rec.dbl_metrics == std::vector<double>{3.3, 4.4}));
    CHECK(!reader.next(rec));

#ifdef JAC313_HAS_JTEXT_PERSIST
    reader.convert_to_jtext(out.string(), 2, 2);
    CHECK(fs::exists(out.string() + ".jtext"));
#endif

    std::cout << "jac313::Store::v001 BinaryEventLogReader OK\n";
    return EXIT_SUCCESS;
}