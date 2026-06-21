#include <cstdlib>
#include <cstddef>

import jac313.store.v002;

// Reference the exported symbols by their FULLY-QUALIFIED names — exactly what
// bare `export using` broke (it injected them into the global namespace instead
// of jac313::Store::v002::). Covers the core store (ts_store, ts_store_config,
// PersistedEvent, PersistMode) and the persistence layer (BinaryRecord,
// BinaryEventLogReader, SqlEventSink) plus a Qlite-backed alias (SqliteError).
// The old bare-import smoke referenced no symbol, so it could not catch this.
int main() {
    // ts_store / ts_store_config are class templates; reference the concrete
    // exports — the export-namespace wrapping is uniform, so these prove it for all.
    (void)sizeof(jac313::Store::v002::PersistedEvent);
    (void)sizeof(jac313::Store::v002::BinaryRecord);
    (void)sizeof(jac313::Store::v002::BinaryEventLogReader);
    (void)sizeof(jac313::Store::v002::SqlEventSink);
    (void)sizeof(jac313::Store::v002::SqliteError);

    const jac313::Store::v002::PersistMode pm = jac313::Store::v002::PersistMode::All;
    return (pm == jac313::Store::v002::PersistMode::All) ? EXIT_SUCCESS : EXIT_FAILURE;
}
