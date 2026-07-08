module;

#include <jac313/Store/v001/headers/ts_store.hpp>
#include <jac313/Store/v001/headers/ts_store_config.hpp>
#include <jac313/Store/v001/headers/persistence/PersistCommon.hpp>
#include <jac313/Store/v001/headers/persistence/BinaryEventLogReader.hpp>
#include <jac313/Store/v001/headers/persistence/BinaryEventSink.hpp>
#include <jac313/Store/v001/headers/persistence/FlagRoutingEventSink.hpp>
#include <jac313/Store/v001/headers/persistence/HtmlEventSink.hpp>
#include <jac313/Store/v001/headers/persistence/JsonEventLogReader.hpp>
#include <jac313/Store/v001/headers/persistence/JsonEventSink.hpp>
#include <jac313/Store/v001/headers/persistence/JTextEventSink.hpp>
#include <jac313/Store/v001/headers/persistence/SqlEventSink.hpp>
#include <jac313/Store/v001/headers/metric_view.hpp>

export module jac313.store.v001;

// Re-export under the namespace, not bare `export using` (which injects the names
// into the GLOBAL namespace and diverges from the header's jac313::Store::v001::
// spelling). Importers then see the symbols qualified, matching the header + Qlite + jText.
export namespace jac313::Store::v001 {
    using jac313::Store::v001::version;
    using jac313::Store::v001::bounded_string;
    using jac313::Store::v001::ts_store;
    using jac313::Store::v001::ts_store_config;
    using jac313::Store::v001::DoubleBufferedWriter;
    using jac313::Store::v001::IEventSink;
    using jac313::Store::v001::PersistedEvent;
    using jac313::Store::v001::PersistMode;
    using jac313::Store::v001::BinaryRecord;
    using jac313::Store::v001::BinaryEventLogReader;
    using jac313::Store::v001::BinaryEventSink;
    using jac313::Store::v001::FlagRoutingEventSink;
    using jac313::Store::v001::HtmlEventSink;
    using jac313::Store::v001::JsonEventLogReader;
    using jac313::Store::v001::JsonRecord;
    using jac313::Store::v001::JsonEventSink;
    using jac313::Store::v001::JTextEventSink;
    using jac313::Store::v001::SqlEventSink;
    using jac313::Store::v001::Sqlite;
    using jac313::Store::v001::SqliteError;
    using jac313::Store::v001::MetricSummary;
    using jac313::Store::v001::EventMetricSummary;
    using jac313::Store::v001::summarize_metrics;
    using jac313::Store::v001::metric_view;
    using jac313::Store::v001::StoreFlags;
    using jac313::Store::v001::set_user_flag;
    using jac313::Store::v001::set_internal_flag;
    using jac313::Store::v001::set_metric_flag;
    using jac313::Store::v001::set_severity;
    using jac313::Store::v001::flags_set_has_data;
    using jac313::Store::v001::flags_clear_has_data;
}