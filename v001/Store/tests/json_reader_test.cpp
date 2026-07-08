#include <jac313/Store/v001.hpp>
#include <jac313/Store/v001/headers/persistence/JsonEventLogReader.hpp>
#include <jac313/Store/v001/headers/persistence/JsonEventSink.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
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
    const fs::path out = fs::temp_directory_path() / "jac313_store_json_reader_test";
    for (const auto& suffix : {"", "_Ints", "_Floats"}) {
        fs::remove(out.string() + suffix + ".json");
    }

    auto sink = std::make_unique<jac313::Store::v001::JsonEventSink>(
        out.string(), 2, 2, jac313::Store::v001::PersistMode::All);
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

    const std::string main_path = out.string() + ".json";
    CHECK(fs::exists(main_path));

    jac313::Store::v001::JsonEventLogReader reader(main_path);
    jac313::Store::v001::JsonRecord rec{};
    CHECK(reader.next(rec));
    CHECK(rec.event_id == 99);
    CHECK(rec.thread_id == 1);
    CHECK(rec.per_thread_event_id == 7);
    CHECK(rec.category == "reader-test");
    CHECK(rec.payload == "round-trip");
    CHECK(!reader.next(rec));

    jac313::Store::v001::JsonEventLogReader ints_reader(out.string() + "_Ints.json");
    jac313::Store::v001::JsonRecord ints_rec{};
    CHECK(ints_reader.next(ints_rec));
    CHECK((ints_rec.int_metrics == std::vector<int64_t>{11, 22}));

    jac313::Store::v001::JsonEventLogReader floats_reader(out.string() + "_Floats.json");
    jac313::Store::v001::JsonRecord floats_rec{};
    CHECK(floats_reader.next(floats_rec));
    CHECK((floats_rec.dbl_metrics == std::vector<double>{3.3, 4.4}));

    // Malformed line must halt the reader (no recovery).
    {
        const fs::path bad = fs::temp_directory_path() / "jac313_store_json_reader_bad";
        fs::remove(bad.string() + ".json");
        std::ofstream ofs(bad.string() + ".json");
        ofs << "//File: bad\n//\n";
        ofs << R"({"id":1,"thread_id":0,"per_thread_event_id":0,"flags_raw":"0x1","category":"x","payload":"y","timestamp_us":1})" << "\n";
        ofs << "not json at all\n";
        ofs.close();

        jac313::Store::v001::JsonEventLogReader bad_reader(bad.string() + ".json");
        jac313::Store::v001::JsonRecord bad_rec{};
        CHECK(bad_reader.next(bad_rec));
        CHECK(bad_rec.event_id == 1);
        CHECK(!bad_reader.next(bad_rec));
    }

    std::cout << "jac313::Store::v001 JSON reader test OK\n";
    return EXIT_SUCCESS;
}