module;

#include <jac313/Store/v002/headers/ts_store.hpp>
#include <jac313/Store/v002/headers/ts_store_config.hpp>
#include <jac313/Store/v002/headers/persistence/PersistCommon.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventLogReader.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/FlagRoutingEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/HtmlEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/JTextEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/SqlEventSink.hpp>
#include <jac313/Store/v002/headers/metric_view.hpp>

export module jac313.store.v002;

// Re-export under the namespace, not bare `export using` (which injects the names
// into the GLOBAL namespace and diverges from the header's jac313::Store::v002::
// spelling). Importers then see the symbols qualified, matching the header + Qlite + jText.
export namespace jac313::Store::v002 {
    using jac313::Store::v002::version;
    using jac313::Store::v002::bounded_string;
    using jac313::Store::v002::ts_store;
    using jac313::Store::v002::ts_store_config;
    using jac313::Store::v002::DoubleBufferedWriter;
    using jac313::Store::v002::IEventSink;
    using jac313::Store::v002::PersistedEvent;
    using jac313::Store::v002::PersistMode;
    using jac313::Store::v002::BinaryRecord;
    using jac313::Store::v002::BinaryEventLogReader;
    using jac313::Store::v002::BinaryEventSink;
    using jac313::Store::v002::FlagRoutingEventSink;
    using jac313::Store::v002::HtmlEventSink;
    using jac313::Store::v002::JTextEventSink;
    using jac313::Store::v002::SqlEventSink;
    using jac313::Store::v002::Sqlite;
    using jac313::Store::v002::SqliteError;
    using jac313::Store::v002::MetricSummary;
    using jac313::Store::v002::EventMetricSummary;
    using jac313::Store::v002::summarize_metrics;
    using jac313::Store::v002::metric_view;
    using jac313::Store::v002::StoreFlags;
    using jac313::Store::v002::set_user_flag;
    using jac313::Store::v002::set_internal_flag;
    using jac313::Store::v002::set_metric_flag;
    using jac313::Store::v002::set_severity;
    using jac313::Store::v002::flags_set_has_data;
    using jac313::Store::v002::flags_clear_has_data;
}