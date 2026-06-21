#include <jac313/Qlite/v001.hpp>

#include <iostream>
#include <optional>
#include <string>

int main() {
    try {
        jac313::Qlite::v001::Sqlite db(":memory:");

        db.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
        db.exec("INSERT INTO users (name, age) VALUES (?, ?)", std::string("Alice"), 30);

        auto stmt = db.prepare("SELECT id, name, age FROM users");
        while (stmt.step()) {
            int64_t id;
            std::string name;
            std::optional<int64_t> age;
            stmt.get(id, name, age);
            std::cout << id << ": " << name << " (age=" << (age ? std::to_string(*age) : "null") << ")\n";
        }
    } catch (const jac313::Qlite::v001::SqliteError& e) {
        std::cerr << "SQLite error: " << e.what() << " (code " << e.code() << ")\n";
        return 1;
    }
    return 0;
}