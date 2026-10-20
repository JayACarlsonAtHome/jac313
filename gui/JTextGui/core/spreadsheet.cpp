// Excel (.xlsx) files: a zip of a few XML parts. Written with stored (uncompressed) zip
// entries, which Excel and LibreOffice read; read with zlib for the deflated entries
// other programs write. The XML handled here is only what a worksheet needs, so a small
// scanner does instead of a full XML library.

#include "jtext_gui/core/spreadsheet.hpp"

#include <zlib.h>

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <sstream>

namespace jtext_gui {

namespace {

// --- small text helpers -----------------------------------------------------------------

std::string lower(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

std::string trim(std::string_view s) {
    std::size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return std::string(s.substr(a, b - a));
}

// Text for XML: the five special characters escaped; control characters that XML 1.0 does
// not allow (all below 0x20 except tab, line feed, carriage return) are dropped.
std::string xml_escape(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (char ch : s) {
        const auto u = static_cast<unsigned char>(ch);
        switch (ch) {
        case '&': out += "&amp;"; break;
        case '<': out += "&lt;"; break;
        case '>': out += "&gt;"; break;
        case '"': out += "&quot;"; break;
        case '\'': out += "&apos;"; break;
        default:
            if (u < 0x20 && ch != '\t' && ch != '\n' && ch != '\r') break;
            out += ch;
        }
    }
    return out;
}

void append_utf8(std::string& out, std::uint32_t cp) {
    if (cp < 0x80) out += static_cast<char>(cp);
    else if (cp < 0x800) { out += static_cast<char>(0xC0 | (cp >> 6)); out += static_cast<char>(0x80 | (cp & 0x3F)); }
    else if (cp < 0x10000) {
        out += static_cast<char>(0xE0 | (cp >> 12));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    } else {
        out += static_cast<char>(0xF0 | (cp >> 18));
        out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
}

std::string xml_unescape(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] != '&') { out += s[i]; continue; }
        const auto semi = s.find(';', i);
        if (semi == std::string_view::npos) { out += s[i]; continue; }
        const std::string_view ent = s.substr(i + 1, semi - i - 1);
        if (ent == "amp") out += '&';
        else if (ent == "lt") out += '<';
        else if (ent == "gt") out += '>';
        else if (ent == "quot") out += '"';
        else if (ent == "apos") out += '\'';
        else if (!ent.empty() && ent[0] == '#') {
            std::uint32_t cp = 0;
            const bool hex = ent.size() > 1 && (ent[1] == 'x' || ent[1] == 'X');
            const auto digits = ent.substr(hex ? 2 : 1);
            std::from_chars(digits.data(), digits.data() + digits.size(), cp, hex ? 16 : 10);
            append_utf8(out, cp);
        } else {
            out.append(s.substr(i, semi - i + 1));
        }
        i = semi;
    }
    return out;
}

// --- a minimal XML scanner: elements by name, their attributes and text --------------------

struct Element {
    std::size_t start = 0;      // '<' of the start tag
    std::size_t content = 0;    // just after the start tag
    std::size_t end = 0;        // '<' of the end tag (== content for an empty element)
    std::size_t after = 0;      // just after the whole element
    std::string tag;            // the start tag's text, for attributes
    bool empty = false;
};

// The next element named name at or after pos ("c" does not match "col"). Elements of the
// same name are not nested in the parts read here.
std::optional<Element> next_element(std::string_view xml, std::string_view name, std::size_t pos) {
    for (;;) {
        const auto lt = xml.find('<', pos);
        if (lt == std::string_view::npos) return std::nullopt;
        std::size_t p = lt + 1;
        // a namespace prefix (x:c) is allowed
        const auto colon_end = xml.find_first_of(" \t\r\n/>", p);
        if (colon_end == std::string_view::npos) return std::nullopt;
        std::string_view qname = xml.substr(p, colon_end - p);
        if (const auto c = qname.find(':'); c != std::string_view::npos) qname = qname.substr(c + 1);
        if (qname != name || xml[p] == '/') { pos = lt + 1; continue; }
        const auto gt = xml.find('>', p);
        if (gt == std::string_view::npos) return std::nullopt;
        Element e;
        e.start = lt;
        e.tag = std::string(xml.substr(lt, gt - lt + 1));
        e.empty = xml[gt - 1] == '/';
        e.content = gt + 1;
        if (e.empty) {
            e.end = e.content;
            e.after = e.content;
            return e;
        }
        // the matching end tag (with or without the same prefix)
        std::size_t q = e.content;
        for (;;) {
            const auto close = xml.find("</", q);
            if (close == std::string_view::npos) return std::nullopt;
            const auto cgt = xml.find('>', close);
            if (cgt == std::string_view::npos) return std::nullopt;
            std::string_view cname = xml.substr(close + 2, cgt - close - 2);
            if (const auto c = cname.find(':'); c != std::string_view::npos) cname = cname.substr(c + 1);
            if (trim(cname) == name) {
                e.end = close;
                e.after = cgt + 1;
                return e;
            }
            q = cgt + 1;
        }
    }
}

std::string attribute(const std::string& tag, std::string_view name) {
    std::size_t pos = 0;
    while ((pos = tag.find(name, pos)) != std::string::npos) {
        const bool start_ok = pos > 0 && std::isspace(static_cast<unsigned char>(tag[pos - 1]));
        std::size_t p = pos + name.size();
        while (p < tag.size() && std::isspace(static_cast<unsigned char>(tag[p]))) ++p;
        if (start_ok && p < tag.size() && tag[p] == '=') {
            ++p;
            while (p < tag.size() && std::isspace(static_cast<unsigned char>(tag[p]))) ++p;
            if (p < tag.size() && (tag[p] == '"' || tag[p] == '\'')) {
                const char quote = tag[p];
                const auto close = tag.find(quote, p + 1);
                if (close != std::string::npos) return xml_unescape(std::string_view(tag).substr(p + 1, close - p - 1));
            }
        }
        pos += name.size();
    }
    return {};
}

// The text of every <t> inside [from, to), skipping phonetic runs (<rPh>).
std::string gather_t(std::string_view xml, std::size_t from, std::size_t to) {
    std::string out;
    const std::string_view part = xml.substr(from, to - from);
    std::size_t pos = 0;
    while (auto t = next_element(part, "t", pos)) {
        const auto rph = part.rfind("<rPh", t->start);
        const auto rph_end = rph == std::string_view::npos ? std::string_view::npos : part.find("</rPh>", rph);
        const bool phonetic = rph != std::string_view::npos && rph_end != std::string_view::npos && rph_end > t->start;
        if (!phonetic) out += xml_unescape(part.substr(t->content, t->end - t->content));
        pos = t->after;
    }
    return out;
}

// --- zip ------------------------------------------------------------------------------------

void put16(std::string& b, std::uint16_t v) { b += static_cast<char>(v & 0xFF); b += static_cast<char>(v >> 8); }
void put32(std::string& b, std::uint32_t v) { put16(b, static_cast<std::uint16_t>(v & 0xFFFF)); put16(b, static_cast<std::uint16_t>(v >> 16)); }
std::uint16_t get16(std::string_view b, std::size_t p) {
    return static_cast<std::uint16_t>(static_cast<unsigned char>(b[p]) | (static_cast<unsigned char>(b[p + 1]) << 8));
}
std::uint32_t get32(std::string_view b, std::size_t p) {
    return get16(b, p) | (static_cast<std::uint32_t>(get16(b, p + 2)) << 16);
}

struct ZipEntry { std::string name; std::string data; };

// A zip with stored (uncompressed) entries.
std::string zip_store(const std::vector<ZipEntry>& entries) {
    std::string out, central;
    for (const auto& e : entries) {
        const auto crc = static_cast<std::uint32_t>(crc32(0L, reinterpret_cast<const Bytef*>(e.data.data()),
                                                          static_cast<uInt>(e.data.size())));
        const auto offset = static_cast<std::uint32_t>(out.size());
        const auto size = static_cast<std::uint32_t>(e.data.size());
        const auto nlen = static_cast<std::uint16_t>(e.name.size());
        put32(out, 0x04034b50); put16(out, 20); put16(out, 0x0800); put16(out, 0);   // UTF-8 names, stored
        put16(out, 0); put16(out, 0x21);                                              // time, date (1980-01-01)
        put32(out, crc); put32(out, size); put32(out, size); put16(out, nlen); put16(out, 0);
        out += e.name;
        out += e.data;
        put32(central, 0x02014b50); put16(central, 20); put16(central, 20); put16(central, 0x0800); put16(central, 0);
        put16(central, 0); put16(central, 0x21);
        put32(central, crc); put32(central, size); put32(central, size); put16(central, nlen);
        put16(central, 0); put16(central, 0); put16(central, 0); put16(central, 0); put32(central, 0);
        put32(central, offset);
        central += e.name;
    }
    const auto cd_offset = static_cast<std::uint32_t>(out.size());
    out += central;
    put32(out, 0x06054b50); put16(out, 0); put16(out, 0);
    put16(out, static_cast<std::uint16_t>(entries.size())); put16(out, static_cast<std::uint16_t>(entries.size()));
    put32(out, static_cast<std::uint32_t>(central.size())); put32(out, cd_offset); put16(out, 0);
    return out;
}

std::expected<std::string, std::string> inflate_raw(std::string_view in, std::size_t expected_size) {
    std::string out(expected_size, '\0');
    z_stream zs{};
    if (inflateInit2(&zs, -MAX_WBITS) != Z_OK) return std::unexpected("zlib: cannot start inflate");
    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(in.data()));
    zs.avail_in = static_cast<uInt>(in.size());
    zs.next_out = reinterpret_cast<Bytef*>(out.data());
    zs.avail_out = static_cast<uInt>(out.size());
    const int rc = inflate(&zs, Z_FINISH);
    inflateEnd(&zs);
    if (rc != Z_STREAM_END || zs.total_out != expected_size) return std::unexpected("damaged zip entry");
    return out;
}

// Every entry of a zip, by name (stored or deflated; no zip64).
std::expected<std::map<std::string, std::string>, std::string> unzip(std::string_view z) {
    if (z.size() < 22) return std::unexpected("not a zip file (too short)");
    std::size_t eocd = std::string_view::npos;
    for (std::size_t p = z.size() - 22 + 1; p-- > 0 && z.size() - p <= 22 + 65535;)
        if (get32(z, p) == 0x06054b50) { eocd = p; break; }
    if (eocd == std::string_view::npos) return std::unexpected("not a zip file (no directory)");
    const std::uint16_t count = get16(z, eocd + 10);
    std::size_t p = get32(z, eocd + 16);
    std::map<std::string, std::string> out;
    for (std::uint16_t i = 0; i < count; ++i) {
        if (p + 46 > z.size() || get32(z, p) != 0x02014b50) return std::unexpected("damaged zip directory");
        const std::uint16_t method = get16(z, p + 10);
        const std::uint32_t csize = get32(z, p + 20), usize = get32(z, p + 24);
        const std::uint16_t nlen = get16(z, p + 28), xlen = get16(z, p + 30), clen = get16(z, p + 32);
        const std::uint32_t local = get32(z, p + 42);
        if (csize == 0xFFFFFFFF || usize == 0xFFFFFFFF) return std::unexpected("zip64 files are not supported");
        const std::string name(z.substr(p + 46, nlen));
        p += 46 + nlen + xlen + clen;
        if (local + 30 > z.size() || get32(z, local) != 0x04034b50) return std::unexpected("damaged zip entry " + name);
        const std::size_t data = local + 30 + get16(z, local + 26) + get16(z, local + 28);
        if (data + csize > z.size()) return std::unexpected("damaged zip entry " + name);
        const std::string_view raw = z.substr(data, csize);
        if (method == 0) out[name] = std::string(raw);
        else if (method == 8) {
            auto inflated = inflate_raw(raw, usize);
            if (!inflated) return std::unexpected(inflated.error() + ": " + name);
            out[name] = std::move(*inflated);
        } else return std::unexpected("zip entry " + name + " uses an unsupported compression");
    }
    return out;
}

// --- cells ----------------------------------------------------------------------------------

std::string column_letters(std::size_t col) {   // 0 -> A, 25 -> Z, 26 -> AA
    std::string s;
    ++col;
    while (col > 0) {
        s.insert(s.begin(), static_cast<char>('A' + (col - 1) % 26));
        col = (col - 1) / 26;
    }
    return s;
}

// "B12" -> column 1; -1 when not a cell reference.
long column_of(std::string_view ref) {
    long col = 0;
    std::size_t i = 0;
    while (i < ref.size() && std::isalpha(static_cast<unsigned char>(ref[i]))) {
        col = col * 26 + (std::toupper(static_cast<unsigned char>(ref[i])) - 'A' + 1);
        ++i;
    }
    return i == 0 ? -1 : col - 1;
}

// A number Excel keeps exactly as written: no leading zeros (other than "0" before a point),
// no exponent, at most 15 significant digits. Anything else is written as text.
bool plain_number(std::string_view s) {
    if (s.empty()) return false;
    std::size_t i = 0;
    if (s[0] == '-') ++i;
    if (i >= s.size()) return false;
    const std::size_t int_start = i;
    while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
    const std::size_t int_len = i - int_start;
    if (int_len == 0 || (int_len > 1 && s[int_start] == '0')) return false;
    std::size_t frac_len = 0;
    if (i < s.size() && s[i] == '.') {
        ++i;
        const std::size_t f = i;
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
        frac_len = i - f;
        if (frac_len == 0) return false;
    }
    if (i != s.size()) return false;
    const std::size_t significant = (s[int_start] == '0' ? 0 : int_len) + frac_len;
    return significant <= 15;
}

// Days from 1970-01-01 (civil calendar).
long days_from_civil(int y, unsigned m, unsigned d) {
    y -= m <= 2;
    const int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097L + static_cast<long>(doe) - 719468L;
}

void civil_from_days(long z, int& y, unsigned& m, unsigned& d) {
    z += 719468;
    const long era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);
    const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    y = static_cast<int>(yoe) + static_cast<int>(era) * 400;
    const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const unsigned mp = (5 * doy + 2) / 153;
    d = doy - (153 * mp + 2) / 5 + 1;
    m = mp + (mp < 10 ? 3 : -9);
    y += m <= 2;
}

// Excel's day numbers count from 1899-12-30 (its 1900 leap-year bug included).
constexpr long kExcelEpoch = -25569;   // days_from_civil(1899, 12, 30)

// "YYYY-MM-DD" -> Excel serial; nullopt when it is not such a date (1900-03-01 and later).
std::optional<long> excel_serial(std::string_view s) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-') return std::nullopt;
    int y = 0;
    unsigned m = 0, d = 0;
    if (std::from_chars(s.data(), s.data() + 4, y).ec != std::errc{}) return std::nullopt;
    if (std::from_chars(s.data() + 5, s.data() + 7, m).ec != std::errc{}) return std::nullopt;
    if (std::from_chars(s.data() + 8, s.data() + 10, d).ec != std::errc{}) return std::nullopt;
    if (y < 1900 || m < 1 || m > 12 || d < 1 || d > 31) return std::nullopt;
    const long days = days_from_civil(y, m, d);
    int y2; unsigned m2, d2;
    civil_from_days(days, y2, m2, d2);
    if (y2 != y || m2 != m || d2 != d) return std::nullopt;   // e.g. 2023-02-30
    const long serial = days - kExcelEpoch;
    if (serial < 61) return std::nullopt;   // before 1900-03-01: Excel's calendar is off there
    return serial;
}

std::string date_from_serial(double serial) {
    const long day = static_cast<long>(std::floor(serial));
    int y; unsigned m, d;
    civil_from_days(day + kExcelEpoch, y, m, d);
    char buf[32];
    const double frac = serial - static_cast<double>(day);
    const long secs = std::lround(frac * 86400.0);
    if (secs > 0 && secs < 86400)
        std::snprintf(buf, sizeof buf, "%04d-%02u-%02u %02ld:%02ld:%02ld", y, m, d, secs / 3600, (secs / 60) % 60, secs % 60);
    else
        std::snprintf(buf, sizeof buf, "%04d-%02u-%02u", y, m, d);
    return buf;
}

bool is_date_format(int id, const std::string& code) {
    if ((id >= 14 && id <= 22) || (id >= 45 && id <= 47)) return true;
    if (code.empty()) return false;
    bool quoted = false, bracket = false;
    for (char c : code) {
        if (c == '"') quoted = !quoted;
        else if (!quoted && c == '[') bracket = true;
        else if (!quoted && c == ']') bracket = false;
        else if (!quoted && !bracket && (c == 'y' || c == 'Y' || c == 'd' || c == 'D')) return true;
    }
    return false;
}

std::expected<std::string, std::string> read_file(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return std::unexpected("cannot read " + path);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

} // namespace

// --- .xlsx writer ----------------------------------------------------------------------------

std::expected<void, std::string> write_xlsx_file(const std::string& path, const Sheet& sheet,
                                                 const std::vector<CellType>& types,
                                                 const std::string& sheet_name) {
    std::size_t cols = 0;
    for (const auto& r : sheet.rows) cols = std::max(cols, r.size());
    // Excel: a sheet name is 1-31 characters without : \ / ? * [ ]
    std::string name;
    for (char c : sheet_name) if (!std::strchr(":\\/?*[]", c)) name += c;
    if (name.empty()) name = "records";
    if (name.size() > 31) name.resize(31);

    std::string ws = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                     "<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\">"
                     "<sheetViews><sheetView workbookViewId=\"0\">"
                     "<pane ySplit=\"1\" topLeftCell=\"A2\" activePane=\"bottomLeft\" state=\"frozen\"/>"
                     "</sheetView></sheetViews>";
    if (cols > 0) {
        ws += "<cols>";
        for (std::size_t c = 0; c < cols; ++c) {
            std::size_t width = 4;
            for (const auto& r : sheet.rows)
                if (c < r.size()) width = std::max(width, std::min<std::size_t>(r[c].size(), 60));
            ws += "<col min=\"" + std::to_string(c + 1) + "\" max=\"" + std::to_string(c + 1) +
                  "\" width=\"" + std::to_string(width + 2) + "\" customWidth=\"1\"/>";
        }
        ws += "</cols>";
    }
    ws += "<sheetData>";
    for (std::size_t r = 0; r < sheet.rows.size(); ++r) {
        ws += "<row r=\"" + std::to_string(r + 1) + "\">";
        for (std::size_t c = 0; c < sheet.rows[r].size(); ++c) {
            const std::string& v = sheet.rows[r][c];
            if (v.empty()) continue;   // an empty cell
            const std::string ref = column_letters(c) + std::to_string(r + 1);
            const CellType t = r == 0 || c >= types.size() ? CellType::Text : types[c];
            // styles: 1 bold header, 2 text, 3 date, 4 number
            if (t == CellType::Number && plain_number(v)) {
                ws += "<c r=\"" + ref + "\" s=\"4\"><v>" + v + "</v></c>";
            } else if (t == CellType::Date && excel_serial(v)) {
                ws += "<c r=\"" + ref + "\" s=\"3\"><v>" + std::to_string(*excel_serial(v)) + "</v></c>";
            } else {
                ws += "<c r=\"" + ref + "\" s=\"" + std::string(r == 0 ? "1" : "2") +
                      "\" t=\"inlineStr\"><is><t xml:space=\"preserve\">" + xml_escape(v) + "</t></is></c>";
            }
        }
        ws += "</row>";
    }
    ws += "</sheetData></worksheet>";

    const std::string styles =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
        "<styleSheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\">"
        "<numFmts count=\"1\"><numFmt numFmtId=\"164\" formatCode=\"yyyy-mm-dd\"/></numFmts>"
        "<fonts count=\"2\"><font><sz val=\"11\"/><name val=\"Calibri\"/></font>"
        "<font><b/><sz val=\"11\"/><name val=\"Calibri\"/></font></fonts>"
        "<fills count=\"2\"><fill><patternFill patternType=\"none\"/></fill>"
        "<fill><patternFill patternType=\"gray125\"/></fill></fills>"
        "<borders count=\"1\"><border><left/><right/><top/><bottom/><diagonal/></border></borders>"
        "<cellStyleXfs count=\"1\"><xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\"/></cellStyleXfs>"
        "<cellXfs count=\"5\">"
        "<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" xfId=\"0\"/>"
        "<xf numFmtId=\"49\" fontId=\"1\" fillId=\"0\" borderId=\"0\" xfId=\"0\" applyFont=\"1\" applyNumberFormat=\"1\"/>"
        "<xf numFmtId=\"49\" fontId=\"0\" fillId=\"0\" borderId=\"0\" xfId=\"0\" applyNumberFormat=\"1\"/>"
        "<xf numFmtId=\"164\" fontId=\"0\" fillId=\"0\" borderId=\"0\" xfId=\"0\" applyNumberFormat=\"1\"/>"
        "<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" xfId=\"0\"/>"
        "</cellXfs>"
        "<cellStyles count=\"1\"><cellStyle name=\"Normal\" xfId=\"0\" builtinId=\"0\"/></cellStyles>"
        "</styleSheet>";

    const std::vector<ZipEntry> entries = {
        {"[Content_Types].xml",
         "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
         "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
         "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
         "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
         "<Override PartName=\"/xl/workbook.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\"/>"
         "<Override PartName=\"/xl/worksheets/sheet1.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>"
         "<Override PartName=\"/xl/styles.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml\"/>"
         "</Types>"},
        {"_rels/.rels",
         "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
         "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
         "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"xl/workbook.xml\"/>"
         "</Relationships>"},
        {"xl/workbook.xml",
         "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
         "<workbook xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" "
         "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">"
         "<sheets><sheet name=\"" + xml_escape(name) + "\" sheetId=\"1\" r:id=\"rId1\"/></sheets></workbook>"},
        {"xl/_rels/workbook.xml.rels",
         "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
         "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
         "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" Target=\"worksheets/sheet1.xml\"/>"
         "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/>"
         "</Relationships>"},
        {"xl/styles.xml", styles},
        {"xl/worksheets/sheet1.xml", ws},
    };
    const std::string zip = zip_store(entries);
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) return std::unexpected("cannot write " + path);
    out.write(zip.data(), static_cast<std::streamsize>(zip.size()));
    if (!out) return std::unexpected("write failed: " + path);
    return {};
}

// --- .xlsx reader ----------------------------------------------------------------------------

std::expected<Sheet, std::string> read_xlsx_file(const std::string& path) {
    auto file = read_file(path);
    if (!file) return std::unexpected(file.error());
    auto parts = unzip(*file);
    if (!parts) return std::unexpected(parts.error());
    auto part = [&](const std::string& n) -> const std::string* {
        auto it = parts->find(n);
        return it == parts->end() ? nullptr : &it->second;
    };

    // the first sheet: workbook.xml names it, its relationships give the part
    std::string sheet_path = "xl/worksheets/sheet1.xml";
    if (const std::string* wb = part("xl/workbook.xml")) {
        if (auto sh = next_element(*wb, "sheet", 0)) {
            std::string rid = attribute(sh->tag, "r:id");
            if (rid.empty()) rid = attribute(sh->tag, "id");
            if (const std::string* rels = part("xl/_rels/workbook.xml.rels")) {
                std::size_t pos = 0;
                while (auto rel = next_element(*rels, "Relationship", pos)) {
                    if (attribute(rel->tag, "Id") == rid) {
                        std::string target = attribute(rel->tag, "Target");
                        sheet_path = target.starts_with("/") ? target.substr(1) : "xl/" + target;
                        break;
                    }
                    pos = rel->after;
                }
            }
        }
    }
    const std::string* ws = part(sheet_path);
    if (!ws) return std::unexpected("no worksheet in " + path);

    std::vector<std::string> shared;
    if (const std::string* ss = part("xl/sharedStrings.xml")) {
        std::size_t pos = 0;
        while (auto si = next_element(*ss, "si", pos)) {
            shared.push_back(si->empty ? std::string() : gather_t(*ss, si->content, si->end));
            pos = si->after;
        }
    }

    // which cell styles are dates
    std::vector<bool> date_style;
    if (const std::string* st = part("xl/styles.xml")) {
        std::map<int, std::string> codes;
        std::size_t pos = 0;
        while (auto nf = next_element(*st, "numFmt", pos)) {
            codes[std::atoi(attribute(nf->tag, "numFmtId").c_str())] = attribute(nf->tag, "formatCode");
            pos = nf->after;
        }
        if (auto xfs = next_element(*st, "cellXfs", 0)) {
            const std::string_view inner = std::string_view(*st).substr(xfs->content, xfs->end - xfs->content);
            std::size_t p = 0;
            while (auto xf = next_element(inner, "xf", p)) {
                const int id = std::atoi(attribute(xf->tag, "numFmtId").c_str());
                auto it = codes.find(id);
                date_style.push_back(is_date_format(id, it == codes.end() ? std::string() : it->second));
                p = xf->after;
            }
        }
    }

    Sheet sheet;
    std::size_t pos = 0;
    long next_row = 1;
    while (auto row = next_element(*ws, "row", pos)) {
        pos = row->after;
        const std::string rref = attribute(row->tag, "r");
        const long rnum = rref.empty() ? next_row : std::atol(rref.c_str());
        next_row = rnum + 1;
        if (rnum < 1 || rnum > 1048576) continue;
        if (sheet.rows.size() < static_cast<std::size_t>(rnum)) sheet.rows.resize(static_cast<std::size_t>(rnum));
        auto& cells = sheet.rows[static_cast<std::size_t>(rnum - 1)];
        if (row->empty) continue;
        const std::string_view inner = std::string_view(*ws).substr(row->content, row->end - row->content);
        std::size_t cp = 0;
        long next_col = 0;
        while (auto c = next_element(inner, "c", cp)) {
            cp = c->after;
            const std::string ref = attribute(c->tag, "r");
            long col = ref.empty() ? next_col : column_of(ref);
            if (col < 0) col = next_col;
            next_col = col + 1;
            if (col > 16383) continue;
            const std::string type = attribute(c->tag, "t");
            const std::string s_attr = attribute(c->tag, "s");
            std::string value;
            if (!c->empty) {
                const std::string_view cin = inner.substr(c->content, c->end - c->content);
                std::string v;
                if (auto ve = next_element(cin, "v", 0)) v = xml_unescape(cin.substr(ve->content, ve->end - ve->content));
                if (type == "s") {
                    const auto i = static_cast<std::size_t>(std::atol(v.c_str()));
                    value = i < shared.size() ? shared[i] : std::string();
                } else if (type == "inlineStr") {
                    if (auto is = next_element(cin, "is", 0)) value = gather_t(cin, is->content, is->end);
                } else if (type == "b") {
                    value = v == "1" ? "TRUE" : "FALSE";
                } else if (type == "str" || type == "e") {
                    value = v;
                } else {   // a number, or a date when its style is a date format
                    value = v;
                    double number = 0;
                    const bool parsed = !v.empty() &&
                        std::from_chars(v.data(), v.data() + v.size(), number).ec == std::errc{};
                    const auto si = static_cast<std::size_t>(std::atol(s_attr.c_str()));
                    if (parsed && !s_attr.empty() && si < date_style.size() && date_style[si]) {
                        value = date_from_serial(number);
                    } else if (parsed) {
                        // Excel stores a double's exact digits (34.95 as 34.950000000000003);
                        // the shortest form that reads back as the same double is what it shows
                        char buf[64];
                        const auto [end, ec] = std::to_chars(buf, buf + sizeof buf, number);
                        if (ec == std::errc{}) value.assign(buf, end);
                    }
                }
            }
            if (cells.size() <= static_cast<std::size_t>(col)) cells.resize(static_cast<std::size_t>(col) + 1);
            cells[static_cast<std::size_t>(col)] = std::move(value);
        }
    }
    while (!sheet.rows.empty() && std::all_of(sheet.rows.back().begin(), sheet.rows.back().end(),
                                              [](const std::string& v) { return v.empty(); }))
        sheet.rows.pop_back();
    return sheet;
}

// --- records <-> sheet -----------------------------------------------------------------------

namespace {
const ColumnDef* field_def(const TableSession& s, std::size_t f) {
    if (f < s.columns.size()) return &s.columns[f];
    if (f < s.field_defs.size()) return &s.field_defs[f];
    return nullptr;
}
} // namespace

std::string field_header(const TableSession& s, std::size_t field) {
    if (const ColumnDef* d = field_def(s, field); d && !d->name.empty()) return d->name;
    if (field < s.column_names.size() && !s.column_names[field].empty()) return s.column_names[field];
    return field == 0 ? std::string("id") : "line " + std::to_string(field + 1);
}

CellType field_cell_type(const TableSession& s, std::size_t field) {
    const ColumnDef* d = field_def(s, field);
    if (!d) return CellType::Text;
    const std::string t = lower(d->jtext_type);
    if (t == "integer" || t == "number" || t == "decimal" || t == "real" || t == "float" || t == "double")
        return CellType::Number;
    if (t == "date") return CellType::Date;
    return CellType::Text;
}

Sheet session_to_sheet(const TableSession& s, const std::vector<std::size_t>& rows,
                       std::vector<std::size_t> fields) {
    fields.erase(std::remove(fields.begin(), fields.end(), std::size_t{0}), fields.end());
    fields.insert(fields.begin(), 0);   // the id first, once
    Sheet sheet;
    auto& header = sheet.rows.emplace_back();
    for (std::size_t f : fields) header.push_back(field_header(s, f));
    for (std::size_t r : rows) {
        if (r >= s.rows.size()) continue;
        auto& out = sheet.rows.emplace_back();
        for (std::size_t f : fields) out.push_back(field_is_null(s.rows[r], f) ? std::string() : field_text(s.rows[r], f));
    }
    return sheet;
}

namespace {
// Two texts that are the same number (149.0 and 149): Excel keeps numbers, not their digits.
bool same_number(const std::string& a, const std::string& b) {
    double x = 0, y = 0;
    const auto ra = std::from_chars(a.data(), a.data() + a.size(), x);
    const auto rb = std::from_chars(b.data(), b.data() + b.size(), y);
    return ra.ec == std::errc{} && ra.ptr == a.data() + a.size() && rb.ec == std::errc{} &&
           rb.ptr == b.data() + b.size() && x == y;
}
} // namespace

std::expected<ImportSummary, std::string> import_sheet(TableSession& s, const Sheet& sheet, bool apply) {
    if (sheet.rows.empty()) return std::unexpected("the sheet is empty");
    const auto& header = sheet.rows[0];

    // the field every column feeds: its name (any case), "line N", or a plain number
    std::size_t known = std::max({s.columns.size(), s.field_defs.size(), s.column_names.size()});
    for (const auto& rec : s.rows) known = std::max(known, rec.size());
    std::map<std::string, std::size_t> by_name;
    for (std::size_t f = 0; f < known; ++f) by_name.emplace(lower(field_header(s, f)), f);
    ImportSummary sum;
    std::vector<std::optional<std::size_t>> target(header.size());
    std::optional<std::size_t> id_col;
    for (std::size_t c = 0; c < header.size(); ++c) {
        const std::string h = lower(trim(header[c]));
        std::optional<std::size_t> f;
        if (auto it = by_name.find(h); it != by_name.end()) f = it->second;
        else {
            std::string_view n = h;
            if (n.starts_with("line ")) n.remove_prefix(5);
            std::size_t num = 0;
            const auto [p, ec] = std::from_chars(n.data(), n.data() + n.size(), num);
            if (ec == std::errc{} && p == n.data() + n.size() && num >= 1 && num <= kMaxFields) f = num - 1;
        }
        if (h == "id") f = 0;
        if (!f) {
            if (!h.empty()) sum.notes.push_back("column \"" + header[c] + "\" ignored (no field of that name)");
            continue;
        }
        if (*f == 0 && !id_col) id_col = c;
        if (*f == 0 && id_col != c) continue;   // a second id column
        target[c] = f;
        ++sum.columns_used;
    }
    if (!id_col) return std::unexpected("no id column: the first row needs a column named \"id\" (or \"line 1\")");

    std::map<std::int64_t, std::size_t> by_id;
    std::set<std::int64_t> added;   // new ids from this sheet
    for (std::size_t r = 0; r < s.rows.size(); ++r)
        if (auto id = record_id_of(s.rows[r])) by_id.emplace(*id, r);

    for (std::size_t r = 1; r < sheet.rows.size(); ++r) {
        const auto& cells = sheet.rows[r];
        auto cell = [&](std::size_t c) -> std::string { return c < cells.size() ? cells[c] : std::string(); };
        if (std::all_of(cells.begin(), cells.end(), [](const std::string& v) { return trim(v).empty(); })) continue;
        const std::string id_text = trim(cell(*id_col));
        const auto id = parse_record_id(id_text);
        if (!id) {
            ++sum.rows_skipped;
            if (sum.rows_skipped <= 5)
                sum.notes.push_back("row " + std::to_string(r + 1) + " skipped: id \"" + id_text + "\" is not an integer");
            continue;
        }
        if (added.count(*id)) {   // a new id that came earlier in the sheet
            sum.notes.push_back("row " + std::to_string(r + 1) + ": id " + id_text +
                                " appears twice among the new records; the later row " +
                                (apply ? "updates it" : "will update it"));
            if (!apply) continue;
        }
        auto found = by_id.find(*id);
        if (found != by_id.end()) {
            const std::size_t row = found->second;
            int changed = 0;
            for (std::size_t c = 0; c < target.size(); ++c) {
                if (!target[c] || *target[c] == 0) continue;
                const std::size_t f = *target[c];
                const std::string v = cell(c);
                const Record& rec = s.rows[row];
                bool same = field_is_null(rec, f) ? v.empty() : field_text(rec, f) == v;
                if (!same && !field_is_null(rec, f) && field_cell_type(s, f) == CellType::Number)
                    same = same_number(field_text(rec, f), v);   // 149.0 in the file, 149 from Excel
                if (same) continue;   // an empty cell over NULL is unchanged
                ++changed;
                if (apply) s.set_field(row, f, v);   // empty cell: an empty string, never NULL
            }
            if (changed) {
                ++sum.records_updated;
                sum.values_changed += changed;
            }
        } else {
            ++sum.records_added;
            if (apply) {
                Record rec;
                rec.fields[0] = std::to_string(*id);
                rec.is_null[0] = false;
                rec.has_line[0] = true;
                const std::size_t row = s.append_row(std::move(rec));
                for (std::size_t c = 0; c < target.size(); ++c)
                    if (target[c] && *target[c] != 0) s.set_field(row, *target[c], cell(c));
                by_id.emplace(*id, row);
            }
            added.insert(*id);
        }
    }
    return sum;
}

} // namespace jtext_gui
