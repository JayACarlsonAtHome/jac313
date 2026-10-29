#include "vendor.hpp"
#include "process.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string_view>

namespace fs = std::filesystem;

namespace jac313::test_cli::vendor {

// ---------------------------------------------------------------- SHA-256 (FIPS 180-4)

namespace {

constexpr std::array<std::uint32_t, 64> kK = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

constexpr std::uint32_t rotr(std::uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }

} // namespace

std::string sha256_hex(const std::string& data) {
    std::uint32_t h[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                          0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    std::string msg = data;
    const std::uint64_t bits = static_cast<std::uint64_t>(data.size()) * 8;
    msg.push_back(static_cast<char>(0x80));
    while (msg.size() % 64 != 56) msg.push_back('\0');
    for (int i = 7; i >= 0; --i) msg.push_back(static_cast<char>((bits >> (i * 8)) & 0xff));
    for (std::size_t off = 0; off < msg.size(); off += 64) {
        std::uint32_t w[64];
        for (int i = 0; i < 16; ++i) {
            const auto* p = reinterpret_cast<const unsigned char*>(msg.data() + off + i * 4);
            w[i] = (std::uint32_t(p[0]) << 24) | (std::uint32_t(p[1]) << 16) | (std::uint32_t(p[2]) << 8) | p[3];
        }
        for (int i = 16; i < 64; ++i) {
            const std::uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            const std::uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }
        std::uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], k = h[7];
        for (int i = 0; i < 64; ++i) {
            const std::uint32_t t1 = k + (rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25)) + ((e & f) ^ (~e & g)) + kK[i] + w[i];
            const std::uint32_t t2 = (rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
            k = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e; h[5] += f; h[6] += g; h[7] += k;
    }
    static const char* hex = "0123456789abcdef";
    std::string out;
    for (std::uint32_t v : h)
        for (int i = 28; i >= 0; i -= 4) out.push_back(hex[(v >> i) & 0xf]);
    return out;
}

// ---------------------------------------------------------------- globs, files

namespace {

bool seg_match(const char* p, const char* s) {   // '*' within one segment
    if (*p == '\0') return *s == '\0';
    if (*p == '*') {
        for (const char* t = s;; ++t) {
            if (seg_match(p + 1, t)) return true;
            if (*t == '\0' || *t == '/') return false;
        }
    }
    return *s != '\0' && *p == *s && seg_match(p + 1, s + 1);   // '/' matches only '/'; '*' never crosses it
}

std::string base_name(const std::string& p) {
    const auto at = p.rfind('/');
    return at == std::string::npos ? p : p.substr(at + 1);
}

std::string slurp(const fs::path& p) {
    std::ifstream in(p, std::ios::binary);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

std::string trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return {};
    return s.substr(b, s.find_last_not_of(" \t\r\n") - b + 1);
}

std::string today() {
    const std::time_t t = std::time(nullptr);
    std::tm tm{};
    localtime_r(&t, &tm);
    char b[16];
    std::strftime(b, sizeof b, "%Y-%m-%d", &tm);
    return b;
}

// Every regular file under root, as root-relative '/' paths, sorted.
std::vector<std::string> walk(const fs::path& root) {
    std::vector<std::string> out;
    std::error_code ec;
    if (!fs::is_directory(root, ec)) return out;
    for (auto it = fs::recursive_directory_iterator(root, ec); it != fs::recursive_directory_iterator(); it.increment(ec)) {
        if (ec) break;
        if (it->is_regular_file(ec)) out.push_back(fs::relative(it->path(), root, ec).generic_string());
    }
    std::sort(out.begin(), out.end());
    return out;
}

std::string git(const fs::path& repo, std::vector<std::string> args) {
    // content only: a permission-bit change is not an uncommitted change
    args.insert(args.begin(), {"git", "-C", repo.string(), "-c", "core.fileMode=false"});
    const auto r = run_process(args, 30, "");
    return r.exit_code == 0 ? trim(r.stdout_text) : std::string{};
}

} // namespace

bool glob_match(const std::string& pattern, const std::string& path) {
    if (pattern.empty()) return false;
    if (pattern.find('/') == std::string::npos) {   // a base-name pattern, or a directory name
        if (seg_match(pattern.c_str(), base_name(path).c_str())) return true;
        std::size_t start = 0;
        for (auto at = path.find('/'); at != std::string::npos; at = path.find('/', start)) {
            if (seg_match(pattern.c_str(), path.substr(start, at - start).c_str())) return true;
            start = at + 1;
        }
        return false;
    }
    if (seg_match(pattern.c_str(), path.c_str())) return true;
    // a pattern naming a directory matches everything under it
    for (auto at = path.find('/'); at != std::string::npos; at = path.find('/', at + 1))
        if (seg_match(pattern.c_str(), path.substr(0, at).c_str())) return true;
    return false;
}

bool matches_any(const std::vector<std::string>& patterns, const std::string& path) {
    for (const auto& p : patterns) if (glob_match(p, path)) return true;
    return false;
}

std::string Manifest::get(const std::string& key) const {
    const auto it = source.find(key);
    return it == source.end() ? std::string{} : it->second;
}

std::vector<std::string> Manifest::list(const std::string& key) const {
    std::vector<std::string> out;
    std::stringstream ss(get(key));
    for (std::string item; std::getline(ss, item, ',');)
        if (!trim(item).empty()) out.push_back(trim(item));
    return out;
}

fs::path projects_dir(const fs::path& start) {
    std::error_code ec;
    fs::path d = fs::absolute(start, ec).lexically_normal();
    if (!fs::is_directory(d, ec)) d = d.parent_path();
    for (int guard = 0; guard < 64 && !d.empty(); ++guard) {
        if (fs::exists(d / ".git", ec)) return d.parent_path();
        if (d == d.root_path()) break;
        d = d.parent_path();
    }
    return {};
}

// ---------------------------------------------------------------- manifest (jText)

namespace {

const char* const kSourceKeys[] = {"canonical_repo", "canonical_dir", "canonical_commit", "canonical_dirty",
                                   "canonical_version", "transform", "ported", "baseline", "local", "exclude",
                                   "notes"};

bool is_null(const std::string& v) { return v == "\x1f" || v == "\\x1F" || v == "\\x1f" || v == "\\N" || v == "NULL"; }

} // namespace

bool read_manifest(const fs::path& file, Manifest& m, std::string& err) {
    m = Manifest{};
    m.path = file;
    std::ifstream in(file);
    if (!in) { err = "cannot read " + file.string(); return false; }
    std::string section;
    bool in_data = false;
    for (std::string line; std::getline(in, line);) {
        const std::string t = trim(line);
        if (t.rfind("=== Section:", 0) == 0) {
            section = trim(t.substr(12, t.find("===", 12) - 12));
            continue;
        }
        if (t == "=== Data ===") { in_data = true; continue; }
        if (t.rfind("===", 0) == 0) { in_data = false; continue; }
        if (!in_data || t.rfind("1. #", 0) != 0 || t.size() < 7 || t[5] != '#') continue;
        const char sep = t[4];
        std::vector<std::string> v;
        std::string body = t.substr(6);
        if (!body.empty() && body[0] == ' ') body.erase(0, 1);
        std::size_t start = 0;
        for (;;) {
            const auto at = body.find(sep, start);
            std::string val = body.substr(start, at == std::string::npos ? std::string::npos : at - start);
            v.push_back(is_null(val) ? std::string{} : val);
            if (at == std::string::npos) break;
            start = at + 1;
        }
        if (section == "source" && v.size() >= 3) m.source[v[1]] = v[2];
        else if (section == "files" && v.size() >= 5) m.files.push_back({v[1], v[2], v[3], v[4]});
    }
    if (m.get("canonical_repo").empty() || m.get("canonical_dir").empty()) {
        err = file.string() + ": no canonical_repo / canonical_dir (not a VENDORED.jtext?)";
        return false;
    }
    return true;
}

std::string manifest_text(const Manifest& m) {
    auto val = [](const std::string& v) { return v.empty() ? std::string("\x1f") : v; };
    std::ostringstream o;
    o << "//File:    VENDORED.jtext\n"
         "//Purpose: Vendored copy of " << m.get("canonical_dir") << " (do not edit the copied files here;\n"
         "//         change the canonical repo, then sync). Drift is reported as warnings, never as failures.\n"
         "//         Written by `jac313_test_cli vendor-sync` / `vendor-baseline` (format: VENDORED_FORMAT.md).\n"
         "//\n"
         "=== jText File ===\n"
         " 1. #?# VENDORED\n"
         " 3. #?# Vendored copy of " << m.get("canonical_dir") << "\n\n\n"
         "=== Section: source ===\n\n\n"
         "=== Fields ===\n"
         " 1. #/# id/Identity(1,1)/Not Null\n"
         " 2. #/# key/String/Not Null\n"
         " 3. #/# value/String\n"
         "=== End Fields ===\n\n\n"
         "=== Data ===\n";
    int id = 0;
    for (const char* k : kSourceKeys) o << "1. #|# " << ++id << "|" << k << "|" << val(m.get(k)) << "\n\n";
    o << "=== End Data ===\n=== End Section ===\n\n\n"
         "=== Section: files ===\n\n\n"
         "=== Fields ===\n"
         " 1. #/# id/Identity(1,1)/Not Null\n"
         " 2. #/# path/String/Not Null             # vendored file, relative to the manifest dir\n"
         " 3. #/# canonical_path/String/Not Null   # canonical file, relative to canonical_dir\n"
         " 4. #/# canonical_sha256/String/Not Null # canonical file content when copied\n"
         " 5. #/# vendored_sha256/String/Not Null  # vendored file content as written\n"
         "=== End Fields ===\n\n\n"
         "=== Data ===\n";
    id = 0;
    for (const auto& f : m.files)
        o << "1. #|# " << ++id << "|" << f.path << "|" << f.canonical_path << "|" << f.canonical_sha256 << "|"
          << f.vendored_sha256 << "\n\n";
    o << "=== End Data ===\n=== End Section ===\n=== End File ===\n";
    return o.str();
}

bool write_manifest(const Manifest& m, std::string& err) {
    const fs::path tmp = m.path.string() + ".tmp";
    {
        std::ofstream out(tmp, std::ios::trunc);
        if (!out) { err = "cannot write " + tmp.string(); return false; }
        out << manifest_text(m);
    }
    std::error_code ec;
    fs::rename(tmp, m.path, ec);
    if (ec) { err = "cannot replace " + m.path.string() + ": " + ec.message(); return false; }
    return true;
}

// ---------------------------------------------------------------- the jac313-prefix transform

namespace {

bool word_char(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; }

std::string lower(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool is_source(const std::string& path) {
    for (const char* ext : {".h", ".hpp", ".cpp", ".inc", ".cppm"})
        if (path.size() > std::strlen(ext) && path.compare(path.size() - std::strlen(ext), std::string::npos, ext) == 0)
            return true;
    return false;
}

// "L.vNNN" at i (3 digits after the v)?
bool versioned_name_at(const std::string& s, std::size_t i, const std::string& lib) {
    if (s.compare(i, lib.size() + 2, lib + ".v") != 0) return false;
    const std::size_t d = i + lib.size() + 2;
    return d + 3 <= s.size() && std::isdigit(static_cast<unsigned char>(s[d])) &&
           std::isdigit(static_cast<unsigned char>(s[d + 1])) && std::isdigit(static_cast<unsigned char>(s[d + 2]));
}

} // namespace

std::string transform_content(const std::string& transform, const std::string& lib, const std::string& path,
                              const std::string& content) {
    if (transform != "jac313-prefix" || !is_source(path)) return content;
    const std::string l = lower(lib);
    std::string out;
    out.reserve(content.size() + content.size() / 16);
    for (std::size_t i = 0; i < content.size();) {
        const char prev = i ? content[i - 1] : '\0';
        if (content.compare(i, lib.size() + 2, lib + "::") == 0 && !word_char(prev) && prev != ':') {
            out += "jac313::" + lib + "::";
            i += lib.size() + 2;
        } else if ((content[i] == '<' || content[i] == '"') && content.compare(i + 1, lib.size() + 1, lib + "/") == 0) {
            out += content[i];
            out += "jac313/" + lib + "/";
            i += lib.size() + 2;
        } else if (!word_char(prev) && versioned_name_at(content, i, lib)) {
            out += "jac313." + l + ".v";
            i += lib.size() + 2;
        } else {
            out += content[i++];
        }
    }
    return out;
}

std::string transform_path(const std::string& transform, const std::string& lib, const std::string& path) {
    if (transform != "jac313-prefix") return path;
    std::string p = path;
    const std::string inc = "include/" + lib + "/";
    if (p.rfind(inc, 0) == 0) p = "include/jac313/" + lib + "/" + p.substr(inc.size());
    const auto slash = p.rfind('/');
    const std::size_t b = slash == std::string::npos ? 0 : slash + 1;
    if (versioned_name_at(p, b, lib)) p = p.substr(0, b) + "jac313." + lower(lib) + ".v" + p.substr(b + lib.size() + 2);
    return p;
}

// ---------------------------------------------------------------- nested libraries

namespace {

// A library the canonical copy itself brings in: a sub-folder with its own VENDORED.jtext
// (jText carries Qlite at jText/vNNN/Qlite). Its paths are renamed for it, and every
// source file of the copy is prefixed for it as well as for the outer library, so a
// consumer's names stay jac313::Qlite inside its copy of jText. The nested manifest is the
// canonical's own bookkeeping and is never copied.
struct Nested {
    std::string dir;   // its folder, relative to the canonical copy
    std::string lib;   // its canonical_repo
};

std::vector<Nested> nested_libs(const fs::path& canon) {
    std::vector<Nested> out;
    std::error_code ec;
    for (const auto& e : fs::directory_iterator(canon, ec)) {
        if (!e.is_directory(ec) || !fs::exists(e.path() / "VENDORED.jtext", ec)) continue;
        Manifest inner;
        std::string err;
        if (read_manifest(e.path() / "VENDORED.jtext", inner, err) && !inner.get("canonical_repo").empty())
            out.push_back({e.path().filename().generic_string(), inner.get("canonical_repo")});
    }
    return out;
}

const Nested* nested_of(const std::vector<Nested>& nested, const std::string& rel) {
    for (const auto& n : nested)
        if (rel.rfind(n.dir + "/", 0) == 0) return &n;
    return nullptr;
}

// Where a canonical file goes in the copy: renamed for the library it belongs to.
std::string copy_path(const std::string& transform, const std::string& lib, const std::vector<Nested>& nested,
                      const std::string& rel) {
    if (const Nested* n = nested_of(nested, rel))
        return n->dir + "/" + transform_path(transform, n->lib, rel.substr(n->dir.size() + 1));
    return transform_path(transform, lib, rel);
}

// A canonical file's content in the copy: prefixed for the outer library and every nested one.
std::string copy_content(const std::string& transform, const std::string& lib, const std::vector<Nested>& nested,
                         const std::string& rel, std::string content) {
    content = transform_content(transform, lib, rel, content);
    for (const auto& n : nested) content = transform_content(transform, n.lib, rel, content);
    return content;
}

} // namespace

// ---------------------------------------------------------------- check

namespace {

struct Findings {
    int warnings = 0;
    void warn(const std::string& s) { std::cout << "vendor-check: WARN " << s << '\n'; ++warnings; }
    void info(const std::string& s) { std::cout << "vendor-check: INFO " << s << '\n'; }
    void ok(const std::string& s) { std::cout << "vendor-check: OK   " << s << '\n'; }
};

std::vector<fs::path> find_manifests(const fs::path& root) {
    std::vector<fs::path> out;
    std::error_code ec;
    for (auto it = fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied, ec);
         it != fs::recursive_directory_iterator(); it.increment(ec)) {
        if (ec) break;
        const std::string name = it->path().filename().string();
        if (it->is_directory(ec) && (name.rfind("build", 0) == 0 || name == ".git" || name == "test-results" ||
                                     name == "kept-logs" || name.rfind("tmp", 0) == 0)) {
            it.disable_recursion_pending();
            continue;
        }
        if (name == "VENDORED.jtext") out.push_back(it->path());
    }
    std::sort(out.begin(), out.end());
    return out;
}

void check_one(const fs::path& manifest_path, Findings& f) {
    Manifest m;
    std::string err;
    if (!read_manifest(manifest_path, m, err)) { f.warn(err); return; }
    const fs::path vroot = manifest_path.parent_path();
    const std::string copy = vroot.filename().string() + " (" + vroot.string() + ")";
    const auto local = m.list("local");
    const auto exclude = m.list("exclude");
    const int before = f.warnings;

    std::set<std::string> listed;
    for (const auto& row : m.files) {
        listed.insert(row.path);
        std::error_code ec;
        const fs::path p = vroot / row.path;
        if (!fs::exists(p, ec)) f.warn(copy + ": " + row.path + " is missing (removed from the copy)");
        else if (sha256_hex(slurp(p)) != row.vendored_sha256)
            f.warn(copy + ": " + row.path + " edited in place (fix it in " + m.get("canonical_repo") + " first)");
    }
    for (const auto& rel : walk(vroot)) {
        if (rel == "VENDORED.jtext" || listed.count(rel) || matches_any(local, rel)) continue;
        f.warn(copy + ": " + rel + " is not from canonical (add it to `local`, or remove it)");
    }

    // a starting line taken over a copy that already differed keeps saying so until a sync
    if (m.get("baseline") == "Yes" && m.get("notes").find("differ") != std::string::npos)
        f.warn(copy + ": baseline only - " + m.get("notes") + " (hand edits or unported changes; "
               "`vendor-sync` settles it)");

    const fs::path canon = projects_dir(vroot) / m.get("canonical_dir");
    std::error_code ec;
    if (!fs::is_directory(canon, ec)) {
        f.info(copy + ": canonical " + canon.string() + " not found here; drift from canonical not checked");
    } else {
        std::vector<std::string> changed;
        std::set<std::string> canon_listed;
        for (const auto& row : m.files) {
            canon_listed.insert(row.canonical_path);
            const fs::path p = canon / row.canonical_path;
            if (!fs::exists(p, ec)) changed.push_back(row.canonical_path + " (removed)");
            else if (sha256_hex(slurp(p)) != row.canonical_sha256) changed.push_back(row.canonical_path);
        }
        for (const auto& rel : walk(canon)) {
            if (canon_listed.count(rel) || matches_any(exclude, rel)) continue;
            // the consumer's own files (CMakeLists.txt, README.md) are not drift
            if (matches_any(local, copy_path(m.get("transform"), m.get("canonical_repo"), nested_libs(canon), rel)))
                continue;
            if (const Nested* n = nested_of(nested_libs(canon), rel); n && rel == n->dir + "/VENDORED.jtext") continue;
            changed.push_back(rel + " (new)");
        }
        if (!changed.empty()) {
            const fs::path repo = projects_dir(vroot) / m.get("canonical_repo");
            const std::string head = git(repo, {"rev-parse", "--short=12", "HEAD"});
            const bool dirty = !git(repo, {"status", "--porcelain", "--", canon.string()}).empty();
            f.warn(copy + ": canonical " + m.get("canonical_repo") + " changed " + std::to_string(changed.size()) +
                   " file(s) since " + m.get("canonical_commit") + " (now " + (head.empty() ? "?" : head) +
                   (dirty ? ", uncommitted changes" : "") + ")");
            const std::size_t show = std::min<std::size_t>(changed.size(), 12);
            for (std::size_t i = 0; i < show; ++i) std::cout << "vendor-check:        " << changed[i] << '\n';
            if (changed.size() > show)
                std::cout << "vendor-check:        ... and " << (changed.size() - show) << " more\n";
        }
    }
    if (f.warnings == before)
        f.ok(copy + ": matches its manifest and " + m.get("canonical_repo") + " " + m.get("canonical_commit") +
             (m.get("baseline") == "Yes" ? " (baseline)" : ""));
}

} // namespace

int run_vendor_check(int argc, char** argv, int first_arg) {
    bool strict = false;
    std::vector<fs::path> manifests;
    for (int i = first_arg; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--strict") strict = true;
        else if (a == "--help" || a == "-h") {
            std::cout << "vendor-check [--strict] [VENDORED.jtext...]\n"
                         "  Drift of the vendored copies (jText, with its Qlite): edited in place, files not from\n"
                         "  canonical, canonical moved ahead. Warnings only: exit 0 (1 with --strict).\n";
            return 0;
        } else manifests.push_back(a);
    }
    if (manifests.empty()) manifests = find_manifests(fs::current_path());
    Findings f;
    if (manifests.empty()) f.info("no VENDORED.jtext under " + fs::current_path().string());
    for (const auto& m : manifests) check_one(m, f);
    std::cout << "vendor-check: " << f.warnings << " warning(s)\n";
    return (strict && f.warnings) ? 1 : 0;
}

// ---------------------------------------------------------------- sync and baseline

namespace {

struct Planned {
    std::string canonical_path, path, content, canonical_sha;
    bool nested = false;   // a file of a library the canonical brings in
};

// The canonical files (minus exclude), transformed. from_head: read the committed content.
std::vector<Planned> plan(const Manifest& m, const fs::path& canon, bool from_head, const fs::path& repo,
                          std::string& err) {
    std::vector<Planned> out;
    const auto exclude = m.list("exclude");
    const auto local = m.list("local");
    const std::string lib = m.get("canonical_repo"), transform = m.get("transform");
    const auto nested = nested_libs(canon);
    std::vector<std::string> files;
    std::string prefix;
    if (from_head) {   // the committed tree of canonical_dir
        prefix = fs::relative(canon, repo).generic_string();
        const auto r = run_process({"git", "-C", repo.string(), "ls-files", "--", prefix}, 60, "");
        std::stringstream ss(r.stdout_text);
        for (std::string line; std::getline(ss, line);)
            if (!line.empty()) files.push_back(line.substr(prefix.size() + 1));
    } else {
        files = walk(canon);
    }
    for (const auto& rel : files) {
        if (matches_any(exclude, rel)) continue;
        const Nested* n = nested_of(nested, rel);
        if (n && rel == n->dir + "/VENDORED.jtext") continue;
        std::string content;
        if (from_head) {
            const auto r = run_process({"git", "-C", repo.string(), "show", "HEAD:" + prefix + "/" + rel}, 60, "");
            if (r.exit_code != 0) { err = "git show HEAD:" + prefix + "/" + rel + " failed"; return {}; }
            content = r.stdout_text;
        } else {
            content = slurp(canon / rel);
        }
        const std::string vpath = copy_path(transform, lib, nested, rel);
        if (matches_any(local, vpath)) continue;
        const std::string sha = sha256_hex(content);
        out.push_back({rel, vpath, copy_content(transform, lib, nested, rel, std::move(content)), sha, n != nullptr});
    }
    return out;
}

// The package version the copied files declare: the "vNNN.NNN" their version() returns (the
// line version-check treats as authoritative). Empty when no copied file defines one, as with
// an editor's own sources; the manifest then holds NULL.
std::string declared_version(const std::vector<Planned>& files) {
    auto is_version = [](std::string_view v) {
        return v.size() == 8 && v[0] == 'v' && v[4] == '.' &&
               std::all_of(v.begin() + 1, v.begin() + 4, [](char c) { return std::isdigit(static_cast<unsigned char>(c)); }) &&
               std::all_of(v.begin() + 5, v.end(), [](char c) { return std::isdigit(static_cast<unsigned char>(c)); });
    };
    for (const auto& p : files) {
        if (p.nested) continue;   // a nested library's version() is its own, not the copy's
        std::istringstream in(p.content);
        for (std::string line; std::getline(in, line);) {
            if (line.find("version()") == std::string::npos || line.find("return") == std::string::npos) continue;
            const auto q1 = line.find('"');
            const auto q2 = q1 == std::string::npos ? q1 : line.find('"', q1 + 1);
            if (q2 == std::string::npos) continue;
            const std::string v = line.substr(q1 + 1, q2 - q1 - 1);
            if (is_version(v)) return v;
        }
    }
    return {};
}

} // namespace

int run_vendor_sync(int argc, char** argv, int first_arg) {
    fs::path manifest_path;
    bool write = false, allow_dirty = false;
    for (int i = first_arg; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--write") write = true;
        else if (a == "--allow-dirty") allow_dirty = true;
        else if (a == "--help" || a == "-h") {
            std::cout << "vendor-sync VENDORED.jtext [--write] [--allow-dirty]\n"
                         "  Forward-port canonical into the vendored copy (copy + transform), then write a\n"
                         "  fresh manifest. Dry run unless --write. Refuses a canonical dir with uncommitted\n"
                         "  changes (PROCESS.md: green and committed first) unless --allow-dirty.\n"
                         "  canonical_version is the version() the copied files declare (NULL when none does).\n";
            return 0;
        } else manifest_path = a;
    }
    Manifest m;
    std::string err;
    if (manifest_path.empty() || !read_manifest(manifest_path, m, err)) {
        std::cerr << "vendor-sync: " << (manifest_path.empty() ? "which VENDORED.jtext?" : err) << '\n';
        return 2;
    }
    const fs::path vroot = fs::absolute(manifest_path).parent_path();
    const fs::path canon = projects_dir(vroot) / m.get("canonical_dir");
    const fs::path repo = projects_dir(vroot) / m.get("canonical_repo");
    if (!fs::is_directory(canon)) { std::cerr << "vendor-sync: no canonical " << canon << '\n'; return 1; }
    const bool dirty = !git(repo, {"status", "--porcelain", "--", canon.string()}).empty();
    if (dirty && !allow_dirty) {
        std::cerr << "vendor-sync: " << canon.string() << " has uncommitted changes; commit them in "
                  << m.get("canonical_repo") << " first (PROCESS.md), or pass --allow-dirty\n";
        return 1;
    }
    auto files = plan(m, canon, false, repo, err);
    if (!err.empty()) { std::cerr << "vendor-sync: " << err << '\n'; return 1; }

    int added = 0, changed = 0, same = 0, removed = 0;
    std::set<std::string> new_paths;
    for (const auto& p : files) {
        new_paths.insert(p.path);
        const fs::path dst = vroot / p.path;
        std::error_code ec;
        const bool exists = fs::exists(dst, ec);
        const bool differs = !exists || slurp(dst) != p.content;
        if (!exists) ++added; else if (differs) ++changed; else ++same;
        if (differs) std::cout << "vendor-sync: " << (exists ? "update " : "add    ") << p.path << '\n';
        if (write && differs) {
            fs::create_directories(dst.parent_path(), ec);
            std::ofstream(dst, std::ios::binary | std::ios::trunc) << p.content;
        }
    }
    for (const auto& row : m.files) {
        if (new_paths.count(row.path)) continue;
        ++removed;
        std::cout << "vendor-sync: remove " << row.path << '\n';
        if (write) { std::error_code ec; fs::remove(vroot / row.path, ec); }
    }
    std::cout << "vendor-sync: " << added << " new, " << changed << " changed, " << removed << " removed, " << same
              << " unchanged" << (write ? "" : " (dry run: nothing written; add --write)") << '\n';
    if (!write) return 0;

    m.source["canonical_commit"] = git(repo, {"rev-parse", "--short=12", "HEAD"});
    m.source["canonical_dirty"] = dirty ? "Yes" : "No";
    m.source["canonical_version"] = declared_version(files);
    m.source["ported"] = today();
    m.source["baseline"] = "No";
    m.files.clear();
    for (const auto& p : files) m.files.push_back({p.path, p.canonical_path, p.canonical_sha, sha256_hex(p.content)});
    m.path = vroot / "VENDORED.jtext";
    if (!write_manifest(m, err)) { std::cerr << "vendor-sync: " << err << '\n'; return 1; }
    std::cout << "vendor-sync: wrote " << m.path.string() << " (bump the package version() in this commit; "
                 "PROCESS.md)\n";
    return 0;
}

int run_vendor_baseline(int argc, char** argv, int first_arg) {
    fs::path vdir;
    Manifest m;
    m.source["transform"] = "jac313-prefix";
    m.source["local"] = "CMakeLists.txt,README.md,VENDORED.jtext";
    m.source["exclude"] = "build,build-*,tools/tests";
    for (int i = first_arg; i < argc; ++i) {
        const std::string a = argv[i];
        auto next = [&]() { return i + 1 < argc ? std::string(argv[++i]) : std::string{}; };
        if (a == "--repo") m.source["canonical_repo"] = next();
        else if (a == "--canonical") m.source["canonical_dir"] = next();
        else if (a == "--transform") m.source["transform"] = next();
        else if (a == "--local") m.source["local"] = next();
        else if (a == "--exclude") m.source["exclude"] = next();
        else if (a == "--version") m.source["canonical_version"] = next();
        else if (a == "--notes") m.source["notes"] = next();
        else if (a == "--help" || a == "-h") {
            std::cout << "vendor-baseline VENDORED_DIR --repo NAME --canonical DIR_UNDER_PROJECTS [--transform T]\n"
                         "                [--local GLOBS] [--exclude GLOBS] [--version V] [--notes TEXT]\n"
                         "  A starting-line manifest for an existing copy: canonical side = its last COMMIT\n"
                         "  (so uncommitted canonical work shows as drift), vendored side = the files as they are.\n"
                         "  --version defaults to the version() the canonical files declare.\n";
            return 0;
        } else vdir = a;
    }
    if (vdir.empty() || m.get("canonical_repo").empty() || m.get("canonical_dir").empty()) {
        std::cerr << "vendor-baseline: VENDORED_DIR, --repo and --canonical are required\n";
        return 2;
    }
    vdir = fs::absolute(vdir).lexically_normal();
    if (vdir.filename().empty()) vdir = vdir.parent_path();
    const fs::path projects = projects_dir(vdir);
    const fs::path canon = projects / m.get("canonical_dir");
    const fs::path repo = projects / m.get("canonical_repo");
    if (!fs::is_directory(canon)) { std::cerr << "vendor-baseline: no canonical " << canon << '\n'; return 1; }
    std::string err;
    const auto files = plan(m, canon, true, repo, err);
    if (!err.empty()) { std::cerr << "vendor-baseline: " << err << '\n'; return 1; }
    m.source["canonical_commit"] = git(repo, {"rev-parse", "--short=12", "HEAD"});
    m.source["canonical_dirty"] = git(repo, {"status", "--porcelain", "--", canon.string()}).empty() ? "No" : "Yes";
    if (m.get("canonical_version").empty()) m.source["canonical_version"] = declared_version(files);
    m.source["ported"] = today();
    m.source["baseline"] = "Yes";
    std::vector<std::string> differ, missing;
    for (const auto& p : files) {
        const fs::path dst = vdir / p.path;
        if (!fs::exists(dst)) { missing.push_back(p.path); continue; }
        const std::string have = slurp(dst);
        if (have != p.content) differ.push_back(p.path);
        m.files.push_back({p.path, p.canonical_path, p.canonical_sha, sha256_hex(have)});
    }
    std::string note = m.get("notes");
    if (!differ.empty() || !missing.empty()) {
        std::string auto_note = "at baseline vs canonical " + m.get("canonical_commit") + ": " +
                                std::to_string(differ.size()) + " file(s) differ after the transform";
        if (!missing.empty()) auto_note += ", " + std::to_string(missing.size()) + " not in the copy";
        note = note.empty() ? auto_note : note + "; " + auto_note;
    }
    for (char& c : note) if (c == '|') c = '/';
    m.source["notes"] = note;
    m.path = vdir / "VENDORED.jtext";
    if (!write_manifest(m, err)) { std::cerr << "vendor-baseline: " << err << '\n'; return 1; }
    std::cout << "vendor-baseline: wrote " << m.path.string() << " (" << m.files.size() << " files; baseline of "
              << m.get("canonical_repo") << " " << m.get("canonical_commit") << ")\n";
    for (const auto& d : differ) std::cout << "vendor-baseline:   differs after transform: " << d << '\n';
    for (const auto& d : missing) std::cout << "vendor-baseline:   not in the copy: " << d << '\n';
    return 0;
}

} // namespace jac313::test_cli::vendor
