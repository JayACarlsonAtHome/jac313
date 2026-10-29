#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/persistence/JsonEventLogReader.hpp>
#include <jac313/Store/v002/headers/persistence/JsonEventSink.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <source_location>
#include <string_view>

namespace fs = std::filesystem;

// A failed check says what and where, and ends the test.
void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    std::exit(EXIT_FAILURE);
}

int main() {
    const fs::path out = fs::temp_directory_path() / "jac313_store_json_reader_test";
    for (const auto& suffix : {"", "_Ints", "_Floats"}) {
        fs::remove(out.string() + suffix + ".json");
    }

    auto sink = std::make_unique<jac313::Store::v002::JsonEventSink>(
        out.string(), 2, 2, jac313::Store::v002::PersistMode::All);
    jac313::Store::v002::DoubleBufferedWriter writer(std::move(sink), 4);

    jac313::Store::v002::PersistedEvent ev{};
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
    check(fs::exists(main_path), "fs::exists(main_path)");

    jac313::Store::v002::JsonEventLogReader reader(main_path);
    jac313::Store::v002::JsonRecord rec{};
    check(reader.next(rec), "reader.next(rec)");
    check(rec.event_id == 99, "rec.event_id == 99");
    check(rec.thread_id == 1, "rec.thread_id == 1");
    check(rec.per_thread_event_id == 7, "rec.per_thread_event_id == 7");
    check(rec.category == "reader-test", "rec.category == \"reader-test\"");
    check(rec.payload == "round-trip", "rec.payload == \"round-trip\"");
    check(!reader.next(rec), "!reader.next(rec)");

    jac313::Store::v002::JsonEventLogReader ints_reader(out.string() + "_Ints.json");
    jac313::Store::v002::JsonRecord ints_rec{};
    check(ints_reader.next(ints_rec), "ints_reader.next(ints_rec)");
    check((ints_rec.int_metrics == std::vector<int64_t>{11, 22}), "(ints_rec.int_metrics == std::vector<int64_t>{11, 22})");

    jac313::Store::v002::JsonEventLogReader floats_reader(out.string() + "_Floats.json");
    jac313::Store::v002::JsonRecord floats_rec{};
    check(floats_reader.next(floats_rec), "floats_reader.next(floats_rec)");
    check((floats_rec.dbl_metrics == std::vector<double>{3.3, 4.4}), "(floats_rec.dbl_metrics == std::vector<double>{3.3, 4.4})");

    // Malformed line must halt the reader (no recovery).
    {
        const fs::path bad = fs::temp_directory_path() / "jac313_store_json_reader_bad";
        fs::remove(bad.string() + ".json");
        std::ofstream ofs(bad.string() + ".json");
        ofs << "//File: bad\n//\n";
        ofs << R"({"id":1,"thread_id":0,"per_thread_event_id":0,"flags_raw":"0x1","category":"x","payload":"y","timestamp_us":1})" << "\n";
        ofs << "not json at all\n";
        ofs.close();

        jac313::Store::v002::JsonEventLogReader bad_reader(bad.string() + ".json");
        jac313::Store::v002::JsonRecord bad_rec{};
        check(bad_reader.next(bad_rec), "bad_reader.next(bad_rec)");
        check(bad_rec.event_id == 1, "bad_rec.event_id == 1");
        check(!bad_reader.next(bad_rec), "!bad_reader.next(bad_rec)");
    }

    std::cout << "jac313::Store::v002 JSON reader test OK\n";
    return EXIT_SUCCESS;
}