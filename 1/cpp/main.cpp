#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "db.h"

static string textPath(const string& file) {
    size_t dot = file.find_last_of('.');
    size_t slash = file.find_last_of("/\\");
    string base = (dot == string::npos || (slash != string::npos && dot < slash)) ? file : file.substr(0, dot);
    string out = base + ".txt";
    if (out == file) out = base + "_view.txt";
    return out;
}

// выполнить одну команду и при изменении сохранить файл данных и txt
static bool runQuery(Database& db, const string& file, const string& query) {
    std::istringstream in(query);
    std::vector<string> cmd;
    string w;
    while (in >> w) cmd.push_back(w);
    if (cmd.empty()) return true;

    bool changed = false;
    if (!dbExecute(db, cmd, changed)) return false;
    if (changed && (!dbSave(db, file) || !dbSaveText(db, textPath(file)))) {
        std::cerr << "ERROR: cannot write file\n";
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    string file = "file.data";
    string query;
    bool hasQuery = false;
    for (int i = 1; i + 1 < argc; i += 2) {
        string key = argv[i];
        if (key == "--file") file = argv[i + 1];
        else if (key == "--query") { query = argv[i + 1]; hasQuery = true; }
    }

    Database db;
    int code = 0;
    if (!dbLoad(db, file)) {
        code = 1;
    } else if (hasQuery) {
        if (!runQuery(db, file, query)) code = 1;
    } else {
        string line;
        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line) || line == "exit") break;
            runQuery(db, file, line);
        }
    }
    dbFree(db);
    return code;
}