module;

#include <jText.h>

export module jac313.jtext.v001.reader;

export import jac313.jtext.v001.core;

// Re-export under the namespace (see core partition for why not bare `export using`).
export namespace jac313::jText::v001 {
    using jac313::jText::v001::JTextFile;
}