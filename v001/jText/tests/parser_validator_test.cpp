// Negative / hardening tests for the jText line parser and structure parser.
//
// The line parser already has dedicated error kinds for the malformed cases
// below; these tests lock that behaviour in (regression guard) and cover the
// newly added rule that a multiline sentinel must be printable ASCII.
//
// Links jText::jText, which exposes the in-tree src/ headers.

#include "parser/line.hpp"
#include "parser/section.hpp"
#include "validator/validator.hpp"

#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace {

int failures = 0;

// Assert parse_line(in) fails with exactly error kind k.
void expect_err(std::string_view in, jtext::parse_error_kind k, const char* what) {
    auto r = jtext::parse_line(in);
    if (r.has_value()) {
        std::cerr << "FAIL[" << what << "]: expected error, parsed OK: \"" << in << "\"\n";
        ++failures;
        return;
    }
    if (r.error().kind != k) {
        std::cerr << "FAIL[" << what << "]: expected " << jtext::to_string(k)
                  << " but got " << jtext::to_string(r.error().kind)
                  << " for \"" << in << "\"\n";
        ++failures;
    }
}

// Assert parse_line(in) succeeds.
void expect_ok(std::string_view in, const char* what) {
    auto r = jtext::parse_line(in);
    if (!r.has_value()) {
        std::cerr << "FAIL[" << what << "]: expected OK, got "
                  << jtext::to_string(r.error().kind) << " for \"" << in << "\"\n";
        ++failures;
    }
}

// Build a minimal valid jText file carrying `date` in a Date field, parse +
// validate it, and report whether validation produced any error. Everything
// else in the document is valid, so an error means the date was rejected.
bool date_validation_errors(const std::string& date,
                            jtext::validation_level level = jtext::validation_level::strict) {
    const std::string doc =
        "=== jText File ===\n"
        " 1. #?# date_test\n"
        " 2. #?# 2026-06-23\n"
        " 3. #?# strict date validation\n"
        " 4. #?# 0.6\n"
        "\n"
        "=== Section: D ===\n"
        "\n"
        "=== Fields ===\n"
        " 1. #/# id/Number/Not Null\n"
        " 2. #/# d/Date/Nullable\n"
        "\n"
        "=== Data ===\n"
        " 1. #?# 1\n"
        " 2. #?# " + date + "\n"
        "\n"
        "=== End Data ===\n"
        "=== End Section ===\n"
        "=== End File ===\n";
    auto pf = jtext::parse_file_structure(doc);
    if (!pf.has_value()) return true;  // structural rejection counts as an error
    return jtext::validate(*pf, level).report.has_errors();
}

void expect_date_ok(const std::string& date) {
    if (date_validation_errors(date)) {
        std::cerr << "FAIL[date-ok]: valid date rejected: \"" << date << "\"\n";
        ++failures;
    }
}
void expect_date_rejected(const std::string& date) {
    if (!date_validation_errors(date)) {
        std::cerr << "FAIL[date-reject]: invalid date accepted: \"" << date << "\"\n";
        ++failures;
    }
}

// Build a 2-field section whose template body is `body`, then parse + validate
// and report whether validation produced any error. Used to exercise the
// template {N} placeholder range check.
bool template_validation_errors(const std::string& body,
                                jtext::validation_level level = jtext::validation_level::strict) {
    const std::string doc =
        "=== jText File ===\n"
        " 1. #?# tmpl_test\n"
        "\n"
        "=== Section: S ===\n"
        "\n"
        "=== Template: T ===\n"
        " 1. <<< !!!T!!!\n"
        + body + "\n"
        "!!!T!!!\n"
        "\n"
        "=== Fields ===\n"
        " 1. #/# id/Number/Not Null\n"
        " 2. #/# b/String/Nullable\n"
        "\n"
        "=== Data ===\n"
        " 1. #?# 1\n"
        " 2. #?# y\n"
        "\n"
        "=== End Data ===\n"
        "=== End Section ===\n"
        "=== End File ===\n";
    auto pf = jtext::parse_file_structure(doc);
    if (!pf.has_value()) return true;
    return jtext::validate(*pf, level).report.has_errors();
}

// A document whose record omits a Not Null field (field 2). The required-field
// check runs at EVERY level, so this must error regardless of level.
bool required_missing_errors(jtext::validation_level level) {
    const std::string doc =
        "=== jText File ===\n"
        " 1. #?# req_test\n"
        "\n"
        "=== Section: S ===\n"
        "\n"
        "=== Fields ===\n"
        " 1. #/# id/Number/Not Null\n"
        " 2. #/# name/String/Not Null\n"
        "\n"
        "=== Data ===\n"
        " 1. #?# 1\n"
        "\n"
        "=== End Data ===\n"
        "=== End Section ===\n"
        "=== End File ===\n";
    auto pf = jtext::parse_file_structure(doc);
    if (!pf.has_value()) return true;
    return jtext::validate(*pf, level).report.has_errors();
}

// ─── record id (field 1) helpers ───
//
// Build a one-section document from a Fields block and a Data block, parse +
// validate it at `level`, and return the result (nullopt if structure parsing
// itself failed).
std::optional<jtext::validate_result> validate_doc(const std::string& fields,
                                                   const std::string& data,
                                                   jtext::validation_level level =
                                                       jtext::validation_level::strict) {
    const std::string doc =
        "=== jText File ===\n"
        " 1. #?# record_id_test\n"
        "\n"
        "=== Section: R ===\n"
        "\n"
        "=== Fields ===\n" + fields +
        "\n"
        "=== Data ===\n" + data +
        "\n"
        "=== End Data ===\n"
        "=== End Section ===\n"
        "=== End File ===\n";
    auto pf = jtext::parse_file_structure(doc);
    if (!pf.has_value()) return std::nullopt;
    return jtext::validate(*pf, level);
}

bool has_issue(const std::optional<jtext::validate_result>& vr, jtext::issue_kind k) {
    if (!vr) return false;
    for (const auto& i : vr->report.issues) if (i.kind == k) return true;
    return false;
}

bool clean(const std::optional<jtext::validate_result>& vr) {
    return vr && !vr->report.has_errors();
}

}  // namespace

int main() {
    using K = jtext::parse_error_kind;

    // --- line numbers ---
    expect_err("0. #?# x",   K::invalid_line_number, "line-number-zero");
    expect_err("100. #?# x", K::invalid_line_number, "line-number-over-99");
    expect_err("x. #?# data", K::invalid_line_number, "line-number-nonnumeric");
    expect_err("1 #?# x",    K::missing_period,      "missing-period");

    // --- formatter rules ---
    expect_err("1. #?$ x", K::formatter_bookends_mismatch,        "bookend-mismatch");
    expect_err("1. ### x", K::formatter_separator_equals_bookend, "sep-equals-bookend");

    // --- hierarchy ---
    expect_err("1. #/# A//B", K::hierarchy_empty_segment, "hierarchy-empty-middle");

    // --- multiline sentinel ---
    expect_err("1. <<< ab", K::multiline_sentinel_invalid, "sentinel-too-short");
    {
        // sentinel with an embedded control byte -> rejected by the new ASCII rule
        std::string ctl = "1. <<< a";
        ctl += '\x01';
        ctl += 'c';
        expect_err(ctl, K::multiline_sentinel_invalid, "sentinel-control-char");
    }

    // --- positives (must still parse) ---
    expect_ok("1. #?# hello world", "ok-simple-data");
    expect_ok("1. #/# A/B/C",       "ok-hierarchy");
    expect_ok("1. <<< !!!END!!!",   "ok-sentinel");

    // --- strict Date validation: exactly YYYY-MM-DD, real calendar date ---
    expect_date_ok("2026-06-23");                 // ordinary valid date
    expect_date_ok("2024-02-29");                 // valid leap day
    expect_date_rejected("2026-02-29");           // Feb 29 in a non-leap year
    expect_date_rejected("2026-13-45");           // month/day out of range
    expect_date_rejected("2026-06-23T10:00:00");  // datetime no longer accepted
    expect_date_rejected("2026/06/23");           // wrong separator
    expect_date_rejected("06-23-2026");           // wrong order / not ISO
    expect_date_rejected("26-06-23");             // 2-digit year

    // --- template {N} placeholder range (section declares 2 fields) ---
    if (template_validation_errors("INSERT VALUES ({1}, {2})")) {
        std::cerr << "FAIL[tmpl-ok]: in-range placeholders rejected\n"; ++failures;
    }
    if (!template_validation_errors("INSERT VALUES ({1}, {3})")) {
        std::cerr << "FAIL[tmpl-range]: {3} accepted with only 2 fields\n"; ++failures;
    }
    if (!template_validation_errors("VALUES ({0})")) {
        std::cerr << "FAIL[tmpl-zero]: {0} accepted\n"; ++failures;
    }

    // ─── PROOF: layered validation levels (same document, different outcomes) ───
    using L = jtext::validation_level;
    auto must = [&](bool cond, const char* what) {
        if (!cond) { std::cerr << "FAIL[layer]: " << what << "\n"; ++failures; }
    };

    // A malformed date is a per-value (Standard+) check:
    //   minimal  -> NOT flagged (fast path skips value parsing)
    //   standard -> flagged
    //   strict   -> flagged
    must(!date_validation_errors("2026-13-45", L::minimal),  "bad date should pass at MINIMAL");
    must( date_validation_errors("2026-13-45", L::standard), "bad date should fail at STANDARD");
    must( date_validation_errors("2026-13-45", L::strict),   "bad date should fail at STRICT");

    // A template {N} out of range is a Strict-only check:
    //   minimal  -> NOT flagged
    //   standard -> NOT flagged
    //   strict   -> flagged
    must(!template_validation_errors("VALUES ({3})", L::minimal),  "tmpl {3} should pass at MINIMAL");
    must(!template_validation_errors("VALUES ({3})", L::standard), "tmpl {3} should pass at STANDARD");
    must( template_validation_errors("VALUES ({3})", L::strict),   "tmpl {3} should fail at STRICT");

    // A missing required (Not Null) field is checked at EVERY level:
    must(required_missing_errors(L::minimal),  "missing-required should fail at MINIMAL");
    must(required_missing_errors(L::standard), "missing-required should fail at STANDARD");
    must(required_missing_errors(L::strict),   "missing-required should fail at STRICT");

    // Sanity: a fully valid document passes cleanly at every level.
    must(!date_validation_errors("2026-06-23", L::minimal),  "valid doc should pass at MINIMAL");
    must(!date_validation_errors("2026-06-23", L::standard), "valid doc should pass at STANDARD");
    must(!date_validation_errors("2026-06-23", L::strict),   "valid doc should pass at STRICT");

    // ─── record id: field 1 is mandatory, any name, always an integer ───
    using IK = jtext::issue_kind;
    const std::string two_fields_data = " 1. #?# 7\n 2. #?# x\n";

    // Any name is fine; Number and every auto designation are integer types.
    for (const char* decl : {" 1. #/# ID/Number\n", " 1. #/# recID/Number/Not Null\n",
                             " 1. #/# FK/AutoID\n", " 1. #/# event_id/AutoNumber\n",
                             " 1. #/# id/Identity(1,1)\n", " 1. #/# id/Serial\n",
                             " 1. #/# id/Number/Identity(100, 5)\n",
                             " 1. #/# id/Number/Not Null/AutoIncrement\n",
                             " 1. #/# id/BigSerial\n"}) {
        auto vr = validate_doc(std::string(decl) + " 2. #/# v/String\n", two_fields_data);
        if (!clean(vr)) { std::cerr << "FAIL[recid-decl-ok]: " << decl; ++failures; }
    }
    // Field 1 must be an integer type and cannot be Nullable.
    must(has_issue(validate_doc(" 1. #/# name/String\n 2. #/# v/String\n", two_fields_data),
                   IK::field_record_id_invalid), "String field 1 must be rejected");
    must(has_issue(validate_doc(" 1. #/# when/Date\n 2. #/# v/String\n",
                                " 1. #?# 2026-01-01\n 2. #?# x\n"),
                   IK::field_record_id_invalid), "Date field 1 must be rejected");
    must(has_issue(validate_doc(" 1. #/# id/Number/Nullable\n 2. #/# v/String\n", two_fields_data),
                   IK::field_record_id_invalid), "Nullable field 1 must be rejected");

    // Auto designation parsing: seed/step and malformed forms.
    {
        auto a = jtext::parse_auto_id_designation("IDENTITY( 100 , 5 )");
        must(a && a->seed == 100 && a->step == 5, "Identity(100,5) seed/step");
        auto b = jtext::parse_auto_id_designation("identity(-1,-1)");
        must(b && b->seed == -1 && b->step == -1, "Identity(-1,-1) seed/step");
        auto c = jtext::parse_auto_id_designation("autoid");
        must(c && c->seed == 1 && c->step == 1, "AutoID defaults to (1,1)");
        must(!jtext::parse_auto_id_designation("Identity(1,0)"), "Identity step 0 rejected");
        must(!jtext::parse_auto_id_designation("Identity(x)"),   "Identity(x) rejected");
        must(!jtext::parse_auto_id_designation("Identity(1,1"),  "unclosed Identity rejected");
        must(!jtext::parse_auto_id_designation("Number"),        "Number is not an auto designation");
    }

    // A record without its id is an error at EVERY level (the producer must make one).
    const std::string id_fields = " 1. #/# id/AutoID\n 2. #/# v/String\n";
    for (auto lvl : {L::minimal, L::standard, L::strict}) {
        must(has_issue(validate_doc(id_fields, " 2. #?# x\n", lvl), IK::record_missing_record_id),
             "record without field 1 must be rejected at every level");
    }
    // Non-integer id: a per-value check (Standard+), like other value formats.
    must(!has_issue(validate_doc(id_fields, " 1. #?# 1.5\n 2. #?# x\n", L::minimal),
                    IK::record_id_not_integer), "non-integer id passes at MINIMAL");
    must(has_issue(validate_doc(id_fields, " 1. #?# 1.5\n 2. #?# x\n", L::standard),
                   IK::record_id_not_integer), "non-integer id rejected at STANDARD");
    must(has_issue(validate_doc(id_fields, " 1. #?# 99999999999999999999\n 2. #?# x\n"),
                   IK::record_id_not_integer), "id beyond 64 bits rejected");

    // Compact rows: the leading number is the FIRST FIELD POSITION, not an id;
    // the id is the first value of a "1. #|# ..." row.
    {
        auto vr = validate_doc(id_fields, " 1. #|# 42|hello\n\n 1. #|# -3|world\n");
        must(clean(vr), "compact rows with ids validate");
        must(vr && vr->file.sections.size() == 1 && vr->file.sections[0].records.size() == 2,
             "two compact records");
        if (vr && vr->file.sections[0].records.size() == 2) {
            const auto& r = vr->file.sections[0].records;
            must(r[0].record_id == 42 && r[1].record_id == -3, "record_id taken from field 1");
            must(r[0].values[1] == std::optional<std::string>{"hello"}, "compact values land in fields 1..N");
        }
        must(has_issue(validate_doc(id_fields, " 2. #|# a|b\n"), IK::record_field_out_of_range),
             "compact row past the last field rejected");
        must(has_issue(validate_doc(" 1. #/# id/AutoID\n 2. #/# v/String\n 3. #/# w/String\n",
                                    " 2. #|# a|b\n"), IK::record_missing_record_id),
             "compact row starting at field 2 has no id");
    }

    // Visible proof table (printed so a direct run shows the layering).
    auto yn = [](bool err) { return err ? "ERROR" : "clean"; };
    std::cout << "\nLayered validation proof — same document, varying --level:\n"
              << "                              minimal   standard  strict\n";
    std::cout << "  bad date '2026-13-45'        "
              << yn(date_validation_errors("2026-13-45", L::minimal))  << "     "
              << yn(date_validation_errors("2026-13-45", L::standard)) << "     "
              << yn(date_validation_errors("2026-13-45", L::strict))   << "\n";
    std::cout << "  template {3} (2 fields)      "
              << yn(template_validation_errors("VALUES ({3})", L::minimal))  << "     "
              << yn(template_validation_errors("VALUES ({3})", L::standard)) << "     "
              << yn(template_validation_errors("VALUES ({3})", L::strict))   << "\n";
    std::cout << "  missing Not Null field       "
              << yn(required_missing_errors(L::minimal))  << "     "
              << yn(required_missing_errors(L::standard)) << "     "
              << yn(required_missing_errors(L::strict))   << "\n";
    std::cout << "  valid document               "
              << yn(date_validation_errors("2026-06-23", L::minimal))  << "     "
              << yn(date_validation_errors("2026-06-23", L::standard)) << "     "
              << yn(date_validation_errors("2026-06-23", L::strict))   << "\n\n";

    if (failures == 0) {
        std::cout << "parser/validator negative tests: all passed\n";
        return EXIT_SUCCESS;
    }
    std::cerr << failures << " parser/validator negative test(s) failed\n";
    return EXIT_FAILURE;
}
