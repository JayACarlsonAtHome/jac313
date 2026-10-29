#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventLogReader.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventSink.hpp>

#include <cstdint>
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
    const fs::path out = fs::temp_directory_path() / "jac313_store_binary_reader_test";
    fs::remove(out.string() + ".bin");
    for (const auto& suffix : {"", "_Ints", "_Floats"}) {
        fs::remove(out.string() + suffix + ".jtext");
    }

    auto sink = std::make_unique<jac313::Store::v002::BinaryEventSink>(
        out.string(), 2, 2, jac313::Store::v002::PersistMode::All, 1 << 20);
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

    const std::string bin_path = out.string() + ".bin";
    check(fs::exists(bin_path), "fs::exists(bin_path)");

    jac313::Store::v002::BinaryEventLogReader reader(bin_path);
    jac313::Store::v002::BinaryRecord rec{};
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

    // Grow correctness: a record larger than a single capacity-doubling must still
    // fit (tiny initial buffer + large payload exercises the multi-doubling grow;
    // before the fix this over-doubled and wrote past the mmap).
    {
        const fs::path big = fs::temp_directory_path() / "jac313_store_binreader_biggrow";
        fs::remove(big.string() + ".bin");
        const std::string big_payload(20000, 'X');
        {
            auto bsink = std::make_unique<jac313::Store::v002::BinaryEventSink>(
                big.string(), 0, 0, jac313::Store::v002::PersistMode::All, 64); // tiny initial buffer
            jac313::Store::v002::DoubleBufferedWriter bwriter(std::move(bsink), 4);
            jac313::Store::v002::PersistedEvent bev{};
            bev.event_id = 1;
            bev.category = "big";
            bev.payload = big_payload;
            bwriter.submit_event(std::move(bev));
            bwriter.finalize();
        }
        jac313::Store::v002::BinaryEventLogReader breader(big.string() + ".bin");
        jac313::Store::v002::BinaryRecord brec{};
        check(breader.next(brec), "breader.next(brec)");
        check(brec.payload == big_payload, "brec.payload == big_payload"); // round-trips => grow fit the large record
        fs::remove(big.string() + ".bin");
    }

    // Hardening: a truncated/malformed record (record_len header claims a body too
    // short to even hold the fixed fields) must be REFUSED, not over-read.
    {
        const fs::path bad = fs::temp_directory_path() / "jac313_store_binreader_malformed.bin";
        {
            std::ofstream f(bad, std::ios::binary);
            const std::uint32_t record_len = 4; // << the 5x uint64 fixed header
            f.write(reinterpret_cast<const char*>(&record_len), sizeof(record_len));
            const char body[4] = {1, 2, 3, 4};
            f.write(body, sizeof(body));
        }
        jac313::Store::v002::BinaryEventLogReader bad_reader(bad.string());
        jac313::Store::v002::BinaryRecord bad_rec{};
        check(!bad_reader.next(bad_rec), "!bad_reader.next(bad_rec)"); // must refuse; must not crash / over-read
        fs::remove(bad);
    }

    // Toughened: huge claimed record_len from table-driven limits perspective.
    // Must refuse cleanly without allocating (was a machine-stall vector<>).
    {
        const fs::path huge = fs::temp_directory_path() / "jac313_store_binreader_huge.bin";
        {
            std::ofstream f(huge, std::ios::binary);
            // Write minimal header
            f << "//File:    test\n//Date:    2026-06-30\n//Purpose: test\n//\n";
            const std::uint32_t bad_len = 500U * 1024 * 1024; // 500 MiB claim
            f.write(reinterpret_cast<const char*>(&bad_len), sizeof(bad_len));
            f.write("x", 1);
        }
        jac313::Store::v002::BinaryEventLogReader huge_reader(huge.string());
        jac313::Store::v002::BinaryRecord hrec{};
        check(!huge_reader.next(hrec), "!huge_reader.next(hrec)"); // refused by kMaxBinaryRecordLen
        fs::remove(huge);
    }

    std::cout << "jac313::Store::v002 BinaryEventLogReader OK (incl. malformed-record refusal + huge-len guard)\n";
    return EXIT_SUCCESS;
}