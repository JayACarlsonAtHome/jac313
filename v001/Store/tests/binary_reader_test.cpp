#include <jac313/Store/v001.hpp>
#include <jac313/Store/v001/headers/persistence/BinaryEventLogReader.hpp>
#include <jac313/Store/v001/headers/persistence/BinaryEventSink.hpp>

#include <cstdlib>
#include <filesystem>
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
    check(fs::exists(bin_path), "fs::exists(bin_path)");

    jac313::Store::v001::BinaryEventLogReader reader(bin_path);
    jac313::Store::v001::BinaryRecord rec{};
    check(reader.next(rec), "reader.next(rec)");
    check(rec.event_id == 99, "rec.event_id == 99");
    check(rec.thread_id == 1, "rec.thread_id == 1");
    check(rec.per_thread_event_id == 7, "rec.per_thread_event_id == 7");
    check(rec.category == "reader-test", "rec.category == \"reader-test\"");
    check(rec.payload == "round-trip", "rec.payload == \"round-trip\"");
    check((rec.int_metrics == std::vector<int64_t>{11, 22}), "(rec.int_metrics == std::vector<int64_t>{11, 22})");
    check((rec.dbl_metrics == std::vector<double>{3.3, 4.4}), "(rec.dbl_metrics == std::vector<double>{3.3, 4.4})");
    check(!reader.next(rec), "!reader.next(rec)");

#ifdef JAC313_HAS_JTEXT_PERSIST
    reader.convert_to_jtext(out.string(), 2, 2);
    check(fs::exists(out.string() + ".jtext"), "fs::exists(out.string() + \".jtext\")");
#endif

    std::cout << "jac313::Store::v001 BinaryEventLogReader OK\n";
    return EXIT_SUCCESS;
}