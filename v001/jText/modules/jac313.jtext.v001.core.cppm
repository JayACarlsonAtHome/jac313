module;

#include <jText.h>

export module jac313.jtext.v001.core;

// Re-export under the namespace, not bare `export using` (which injects the names
// into the GLOBAL namespace and diverges from the header's jac313::jText::v001::
// spelling). Importers then see the symbols qualified, matching the header + Qlite.
export namespace jac313::jText::v001 {
    using jac313::jText::v001::version;
    using jac313::jText::v001::CaseMode;
    using jac313::jText::v001::JTextEntry;
    using jac313::jText::v001::JTextSection;
}