#include <cstdlib>
#include <cstddef>

import jac313.jtext.v001;

// Reference the exported symbols by their FULLY-QUALIFIED names — exactly what
// bare `export using` broke (it injected them into the global namespace instead
// of jac313::jText::v001::). Covers all three partitions: core (CaseMode,
// JTextEntry, JTextSection), reader (JTextFile), writer (JTextProfile,
// JTextWriter, and a free function). The old bare-import smoke referenced no
// symbol, so it could not catch this.
int main() {
    using namespace_ok = jac313::jText::v001::JTextFile;        // reader partition
    (void)sizeof(namespace_ok);
    (void)sizeof(jac313::jText::v001::JTextEntry);              // core
    (void)sizeof(jac313::jText::v001::JTextSection);            // core
    (void)sizeof(jac313::jText::v001::JTextWriter);             // writer

    const jac313::jText::v001::CaseMode cm = jac313::jText::v001::CaseMode::Sensitive;
    const jac313::jText::v001::JTextProfile pf = jac313::jText::v001::JTextProfile::Full;
    const std::size_t w = jac313::jText::v001::jtext_line_pad_width_for_count(42);

    return (cm == jac313::jText::v001::CaseMode::Sensitive
            && pf == jac313::jText::v001::JTextProfile::Full
            && w >= 1)
               ? EXIT_SUCCESS
               : EXIT_FAILURE;
}
