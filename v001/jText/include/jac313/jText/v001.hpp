#pragma once

/**
 * jac313::jText::v001 — the jac313 structured-text library.
 *
 * The implementation now lives in-tree and the public API is declared directly in
 * namespace jac313::jText::v001 (jText.h). This is the public entry point:
 *
 *   #include <jac313/jText/v001.hpp>
 *   jac313::jText::v001::JTextWriter writer("out.jtext");
 *
 * Public symbols: JTextWriter, JTextEntry, JTextSection, JTextFile, CaseMode,
 * JTextProfile, and the write_* / jtext_* free helpers.
 */

#include <jText.h>
