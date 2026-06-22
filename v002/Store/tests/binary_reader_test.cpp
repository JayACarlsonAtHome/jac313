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
    CHECK(fs::exists(bin_path));

    jac313::Store::v002::BinaryEventLogReader reader(bin_path);
    jac313::Store::v002::BinaryRecord rec{};
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
        CHECK(breader.next(brec));
        CHECK(brec.payload == big_payload); // round-trips => grow fit the large record
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
        CHECK(!bad_reader.next(bad_rec)); // must refuse; must not crash / over-read
        fs::remove(bad);
    }

    std::cout << "jac313::Store::v002 BinaryEventLogReader OK (incl. malformed-record refusal)\n";
    return EXIT_SUCCESS;
}