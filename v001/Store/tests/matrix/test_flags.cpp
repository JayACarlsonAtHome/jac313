//tests/ts_store_flags/Test_Flags.cpp
// Standalone unit test for StoreFlags — pure module consumer

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>


#include <jac313/Store/v001/headers/ts_store_flags.hpp>
#include <jac313/Store/v001/headers/impl_details/test_options.hpp>

using namespace jac313::Store::v001;


int main(int argc, char** argv) {
    auto _opts = parse_test_options(argc, argv);
    (void)_opts;  // flags test does not use persist/double-buffer; parse for CLI compatibility with runner
    std::cout << "Running StoreFlags standalone tests...\n\n";

    StoreFlags flags(0);

    // Test user flags
    flags.set(StoreFlags::UserFlag::DatabaseEntry);
    flags.set(StoreFlags::UserFlag::HotCacheHint);
    flags.set(StoreFlags::UserFlag::IsExplicitNull);
    flags.set(StoreFlags::UserFlag::IsResult);
    flags.set(StoreFlags::UserFlag::KeeperRecord);
    flags.set(StoreFlags::UserFlag::LogConsole);
    flags.set(StoreFlags::UserFlag::SendNetwork);

    assert(flags.is_set(StoreFlags::UserFlag::DatabaseEntry));
    assert(flags.is_set(StoreFlags::UserFlag::HotCacheHint));
    assert(flags.is_set(StoreFlags::UserFlag::IsExplicitNull));
    assert(flags.is_set(StoreFlags::UserFlag::IsResult));
    assert(flags.is_set(StoreFlags::UserFlag::KeeperRecord));
    assert(flags.is_set(StoreFlags::UserFlag::LogConsole));
    assert(flags.is_set(StoreFlags::UserFlag::SendNetwork));

    flags.clear(StoreFlags::UserFlag::DatabaseEntry);
    flags.clear(StoreFlags::UserFlag::HotCacheHint);
    flags.clear(StoreFlags::UserFlag::IsExplicitNull);
    flags.clear(StoreFlags::UserFlag::IsResult);
    flags.clear(StoreFlags::UserFlag::KeeperRecord);
    flags.clear(StoreFlags::UserFlag::LogConsole);
    flags.clear(StoreFlags::UserFlag::SendNetwork);

    assert(!flags.is_set(StoreFlags::UserFlag::DatabaseEntry));
    assert(!flags.is_set(StoreFlags::UserFlag::HotCacheHint));
    assert(!flags.is_set(StoreFlags::UserFlag::IsExplicitNull));
    assert(!flags.is_set(StoreFlags::UserFlag::IsResult));
    assert(!flags.is_set(StoreFlags::UserFlag::KeeperRecord));
    assert(!flags.is_set(StoreFlags::UserFlag::LogConsole));
    assert(!flags.is_set(StoreFlags::UserFlag::SendNetwork));

    // Test internal flags
    flags.set(StoreFlags::InternalFlag::HasData);
    flags.set(StoreFlags::InternalFlag::IsInvalid);

    assert(flags.is_set(StoreFlags::InternalFlag::HasData));
    assert(flags.is_set(StoreFlags::InternalFlag::IsInvalid));

    flags.clear(StoreFlags::InternalFlag::HasData);
    flags.clear(StoreFlags::InternalFlag::IsInvalid);

    assert(!flags.is_set(StoreFlags::InternalFlag::HasData));
    assert(!flags.is_set(StoreFlags::InternalFlag::IsInvalid));

    // Test metric flags (bits 18-21)
    flags.set(StoreFlags::MetricFlag::HasIntData);
    flags.set(StoreFlags::MetricFlag::HasIntStats);
    flags.set(StoreFlags::MetricFlag::HasDblData);
    flags.set(StoreFlags::MetricFlag::HasDblStats);

    assert(flags.is_set(StoreFlags::MetricFlag::HasIntData));
    assert(flags.is_set(StoreFlags::MetricFlag::HasIntStats));
    assert(flags.is_set(StoreFlags::MetricFlag::HasDblData));
    assert(flags.is_set(StoreFlags::MetricFlag::HasDblStats));

    flags.clear(StoreFlags::MetricFlag::HasIntData);
    flags.clear(StoreFlags::MetricFlag::HasIntStats);
    flags.clear(StoreFlags::MetricFlag::HasDblData);
    flags.clear(StoreFlags::MetricFlag::HasDblStats);

    assert(!flags.is_set(StoreFlags::MetricFlag::HasIntData));
    assert(!flags.is_set(StoreFlags::MetricFlag::HasIntStats));
    assert(!flags.is_set(StoreFlags::MetricFlag::HasDblData));
    assert(!flags.is_set(StoreFlags::MetricFlag::HasDblStats));

    // Test severity
    flags.set_severity(StoreFlags::Severity::Critical);
    assert(flags.get_severity() == StoreFlags::Severity::Critical);

    flags.set_severity(StoreFlags::Severity::Debug);
    assert(flags.get_severity() == StoreFlags::Severity::Debug);

    flags.set_severity(StoreFlags::Severity::Error);
    assert(flags.get_severity() == StoreFlags::Severity::Error);

    flags.set_severity(StoreFlags::Severity::Fatal);
    assert(flags.get_severity() == StoreFlags::Severity::Fatal);

    flags.set_severity(StoreFlags::Severity::Info);
    assert(flags.get_severity() == StoreFlags::Severity::Info);

    flags.set_severity(StoreFlags::Severity::NotSet);
    assert(flags.get_severity() == StoreFlags::Severity::NotSet);

    flags.set_severity(StoreFlags::Severity::Trace);
    assert(flags.get_severity() == StoreFlags::Severity::Trace);

    flags.set_severity(StoreFlags::Severity::Warn);
    assert(flags.get_severity() == StoreFlags::Severity::Warn);

    flags.clear_severity();
    assert(flags.get_severity() == StoreFlags::Severity::NotSet);

    // Test free-standing helpers
    uint64_t raw = set_user_flag(0, StoreFlags::UserFlag::KeeperRecord);
    raw = set_severity(raw, StoreFlags::Severity::Warn);
    raw = set_metric_flag(raw, StoreFlags::MetricFlag::HasIntData);
    raw = flags_set_has_data(raw);
    StoreFlags flags_from_raw(raw);
    assert(flags_from_raw.is_set(StoreFlags::UserFlag::KeeperRecord));
    assert(flags_from_raw.get_severity() == StoreFlags::Severity::Warn);
    assert(flags_from_raw.is_set(StoreFlags::MetricFlag::HasIntData));
    assert(flags_from_raw.is_set(StoreFlags::InternalFlag::HasData));

    // Test to_string and serialization round-trip with all flag types set
    flags.set(StoreFlags::UserFlag::DatabaseEntry);
    flags.set(StoreFlags::UserFlag::HotCacheHint);
    flags.set(StoreFlags::UserFlag::IsExplicitNull);
    flags.set(StoreFlags::UserFlag::IsResult);
    flags.set(StoreFlags::UserFlag::KeeperRecord);
    flags.set(StoreFlags::UserFlag::LogConsole);
    flags.set(StoreFlags::UserFlag::SendNetwork);

    flags.set(StoreFlags::InternalFlag::HasData);
    flags.set(StoreFlags::InternalFlag::IsInvalid);

    flags.set(StoreFlags::MetricFlag::HasIntData);
    flags.set(StoreFlags::MetricFlag::HasIntStats);
    flags.set(StoreFlags::MetricFlag::HasDblData);
    flags.set(StoreFlags::MetricFlag::HasDblStats);

    flags.set_severity(StoreFlags::Severity::Critical);

    auto bytes = flags.to_bytes();
    StoreFlags flags2(bytes);
    assert(flags2.to_string() == flags.to_string());

    std::cout << "Flags 1: " << flags.to_string() << "\n";
    std::cout << "Flags 2: " << flags2.to_string() << "\n";

    std::cout << "\n\nAll StoreFlags tests PASSED!\n";
    return 0;
}