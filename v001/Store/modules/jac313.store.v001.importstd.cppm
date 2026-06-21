// Module-native jac313.store.v001 — `import std;` front-end (gcc pilot, opt-in via
// JAC313_STORE_IMPORT_STD). One interface unit brings in the whole store API: the std
// library comes from `import std;`, and everything import std does NOT provide is textual
// in the global module fragment below —
//   * the POSIX headers store uses for mmap'd I/O + host sysinfo,
//   * the C-compat headers that declare the GLOBAL names (size_t, ::memcpy, ::strcmp,
//     uint*_t, ...) the header bodies use unqualified, and
//   * the external deps Qlite + jText.
// JAC313_STORE_IMPORT_STD (defined here) makes every guarded store header skip its own
// textual std includes + its `namespace jac313::Store::v001` wrapper, so the bodies paste
// std-free into the single `export namespace` below. The compiled .cpp are included too
// (their own-header re-includes are #pragma once no-ops), so the module is self-contained
// for store's code; it still links jtext_core (JTextWriter) + system SQLite3.

module;

#define JAC313_STORE_IMPORT_STD

#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstdlib>

#include <jac313/Qlite/v001.hpp>
#include <jText.h>

export module jac313.store.v001;

import std;

// Public DECLARATIONS — exported.
export namespace jac313::Store::v001 {
#include <jac313/Store/v001/headers/ts_store.hpp>
#include <jac313/Store/v001/headers/ts_store_config.hpp>
#include <jac313/Store/v001/headers/persistence/PersistCommon.hpp>
#include <jac313/Store/v001/headers/persistence/BinaryEventLogReader.hpp>
#include <jac313/Store/v001/headers/persistence/BinaryEventSink.hpp>
#include <jac313/Store/v001/headers/persistence/FlagRoutingEventSink.hpp>
#include <jac313/Store/v001/headers/persistence/JTextEventSink.hpp>
#include <jac313/Store/v001/headers/persistence/SqlEventSink.hpp>
#include <jac313/Store/v001/headers/metric_view.hpp>
}

// Compiled DEFINITIONS — same module, but NOT exported (the .cpp carry file-local
// constexpr/anon-namespace helpers that have internal linkage and cannot be exported;
// the member definitions still match the exported declarations above). Their #include of
// the headers above are #pragma once no-ops.
namespace jac313::Store::v001 {
#include <jac313/Store/v001/headers/persistence/BinaryEventLogReader.cpp>
#include <jac313/Store/v001/headers/persistence/SqlEventSink.cpp>
#include <jac313/Store/v001/headers/persistence/JTextSplitEventLog.cpp>
#include <jac313/Store/v001/headers/persistence/BinaryEventLogReader_jtext.cpp>
}
