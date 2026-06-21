// jac313::Qlite::v002 — SQLite wrapper IMPLEMENTATION BODY (single source of truth).
//
// This file is an include-fragment, NOT a standalone header: it has no include
// guard, includes nothing itself, and must be included INSIDE the
// `jac313::Qlite::v002` namespace by exactly one of two front-ends:
//
//   * Sqlite.hpp        — textual header for non-module consumers. Provides the
//                         std + <sqlite3.h> declarations via textual #include,
//                         then includes this body inside `namespace { ... }`.
//   * jac313.qlite.v002.cppm (import-std build) — provides std via `import std;`
//                         and <sqlite3.h> via the global module fragment, then
//                         includes this body inside `export namespace { ... }`.
//
// Keeping the implementation here (with no std/sqlite includes of its own) lets
// the textual header and the import-std module share ONE definition: the only
// difference is HOW std and sqlite3 names are made visible to this body.

// Package version, "major.minor": major = the v002 API line, minor = the revision. Bump on
// each small code change (edit the minor); v002 stays until a real (breaking) change, then
// this becomes "v002.001". Enforced by `jac313_test_cli version-check` (pre-push).
[[nodiscard]] constexpr std::string_view version() noexcept { return "v002.001"; }

// Column extractors - one per return type
template<typename T>
T column(sqlite3_stmt* stmt, int col);

template<>
inline std::int64_t column<std::int64_t>(sqlite3_stmt* stmt, int col) {
    return sqlite3_column_int64(stmt, col);
}

template<>
inline double column<double>(sqlite3_stmt* stmt, int col) {
    return sqlite3_column_double(stmt, col);
}

template<>
inline std::string column<std::string>(sqlite3_stmt* stmt, int col) {
    const unsigned char* txt = sqlite3_column_text(stmt, col);
    if (!txt) return {};
    int len = sqlite3_column_bytes(stmt, col);
    return std::string(reinterpret_cast<const char*>(txt), len);
}

template<>
inline std::optional<std::int64_t> column<std::optional<std::int64_t>>(sqlite3_stmt* stmt, int col) {
    if (sqlite3_column_type(stmt, col) == SQLITE_NULL) return std::nullopt;
    return sqlite3_column_int64(stmt, col);
}

template<>
inline std::optional<double> column<std::optional<double>>(sqlite3_stmt* stmt, int col) {
    if (sqlite3_column_type(stmt, col) == SQLITE_NULL) return std::nullopt;
    return sqlite3_column_double(stmt, col);
}

template<>
inline std::optional<std::string> column<std::optional<std::string>>(sqlite3_stmt* stmt, int col) {
    if (sqlite3_column_type(stmt, col) == SQLITE_NULL) return std::nullopt;
    return column<std::string>(stmt, col);
}

// Bind value overloads - one per type (or family)
inline void bind_value(sqlite3_stmt* stmt, int idx, std::int64_t v) {
    sqlite3_bind_int64(stmt, idx, v);
}

inline void bind_value(sqlite3_stmt* stmt, int idx, int v) {
    sqlite3_bind_int64(stmt, idx, v);
}

inline void bind_value(sqlite3_stmt* stmt, int idx, double v) {
    sqlite3_bind_double(stmt, idx, v);
}

inline void bind_value(sqlite3_stmt* stmt, int idx, const std::string& v) {
    sqlite3_bind_text(stmt, idx, v.c_str(), static_cast<int>(v.size()), SQLITE_TRANSIENT);
}

inline void bind_value(sqlite3_stmt* stmt, int idx, const char* v) {
    if (v) sqlite3_bind_text(stmt, idx, v, -1, SQLITE_TRANSIENT);
    else   sqlite3_bind_null(stmt, idx);
}

inline void bind_value(sqlite3_stmt* stmt, int idx, std::nullopt_t) {
    sqlite3_bind_null(stmt, idx);
}

template<typename T>
inline void bind_value(sqlite3_stmt* stmt, int idx, const std::optional<T>& v) {
    if (v.has_value()) {
        bind_value(stmt, idx, *v);
    } else {
        sqlite3_bind_null(stmt, idx);
    }
}

class SqliteError : public std::runtime_error {
public:
    SqliteError(const std::string& msg, int errcode = SQLITE_OK)
        : std::runtime_error(msg + " (code " + std::to_string(errcode) + ")"), code_(errcode) {}
    int code() const { return code_; }
private:
    int code_;
};

class Sqlite {
public:
    explicit Sqlite(const std::string& filename,
                    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE) {
        if (sqlite3_open_v2(filename.c_str(), &db_, flags, nullptr) != SQLITE_OK) {
            std::string msg = sqlite3_errmsg(db_);
            sqlite3_close(db_);
            throw SqliteError("Failed to open database: " + filename + " - " + msg);
        }
    }

    ~Sqlite() {
        if (db_) sqlite3_close(db_);
    }

    Sqlite(const Sqlite&) = delete;
    Sqlite& operator=(const Sqlite&) = delete;

    Sqlite(Sqlite&& other) noexcept : db_(other.db_) { other.db_ = nullptr; }
    Sqlite& operator=(Sqlite&& other) noexcept {
        if (this != &other) {
            if (db_) sqlite3_close(db_);
            db_ = other.db_;
            other.db_ = nullptr;
        }
        return *this;
    }

    void exec(const std::string& sql) {
        char* err = nullptr;
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
            std::string msg = err ? err : "unknown error";
            sqlite3_free(err);
            throw SqliteError("exec failed: " + sql + " - " + msg);
        }
    }

    void begin() { exec("BEGIN TRANSACTION;"); }
    void commit() { exec("COMMIT;"); }
    void rollback() { exec("ROLLBACK;"); }

    class Statement {
    public:
        Statement(Sqlite& db, const std::string& sql) : db_(&db) {
            if (sqlite3_prepare_v2(db.db_, sql.c_str(), -1, &stmt_, nullptr) != SQLITE_OK) {
                throw SqliteError("Failed to prepare: " + sql + " - " + sqlite3_errmsg(db.db_));
            }
        }

        ~Statement() {
            if (stmt_) sqlite3_finalize(stmt_);
        }

        Statement(const Statement&) = delete;
        Statement& operator=(const Statement&) = delete;

        template<typename... Args>
        void bind(Args&&... args) {
            sqlite3_reset(stmt_);
            sqlite3_clear_bindings(stmt_);
            bind_impl(1, std::forward<Args>(args)...);
        }

        bool step() {
            int rc = sqlite3_step(stmt_);
            if (rc == SQLITE_ROW) return true;
            if (rc == SQLITE_DONE) return false;
            throw SqliteError("step failed", rc);
        }

        void reset() {
            sqlite3_reset(stmt_);
        }

        // Indexed binding helpers (for dynamic column counts, e.g. metric tables in SqlEventSink)
        void clear_bindings() {
            sqlite3_clear_bindings(stmt_);
        }

        void bind_int64(int idx, std::int64_t v) {
            sqlite3_bind_int64(stmt_, idx, v);
        }

        void bind_double(int idx, double v) {
            sqlite3_bind_double(stmt_, idx, v);
        }

        // Variadic extraction using the "peel first, recurse on ellipses" pattern
        template<typename... Ts>
        void get(Ts&... values) {
            get_impl(0, values...);
        }

        template<typename... Ts>
        std::tuple<Ts...> get_row() {
            std::tuple<Ts...> row{};
            get_tuple_impl<0>(row);
            return row;
        }

    private:
        void bind_impl(int /*idx*/) {} // base case

        template<typename T, typename... Rest>
        void bind_impl(int idx, T&& val, Rest&&... rest) {
            bind_value(stmt_, idx, std::forward<T>(val));
            bind_impl(idx + 1, std::forward<Rest>(rest)...);
        }

        void get_impl(int /*col*/) {} // base case

        template<typename T, typename... Rest>
        void get_impl(int col, T& val, Rest&... rest) {
            val = column<T>(stmt_, col);
            get_impl(col + 1, rest...);
        }

        template<std::size_t I, typename Tuple>
        void get_tuple_impl(Tuple& t) {
            if constexpr (I < std::tuple_size_v<Tuple>) {
                using T = std::tuple_element_t<I, Tuple>;
                std::get<I>(t) = column<T>(stmt_, static_cast<int>(I));
                get_tuple_impl<I + 1>(t);
            }
        }

        sqlite3_stmt* stmt_ = nullptr;
        Sqlite* db_ = nullptr;
    };

    Statement prepare(const std::string& sql) {
        return Statement(*this, sql);
    }

    template<typename... Args>
    void exec(const std::string& sql, Args&&... args) {
        Statement st(*this, sql);
        st.bind(std::forward<Args>(args)...);
        while (st.step()) {}
    }

private:
    sqlite3* db_ = nullptr;
};
