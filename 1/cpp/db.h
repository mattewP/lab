#pragma once
#include <string>
#include <vector>
#include "structures.h"

// Именованная структура в базе
struct Entry {
    char type;
    string name;
    Array arr;
    SList sl;
    DList dl;
    Stack st;
    Queue qu;
    TNode* root = nullptr;
};

struct Database {
    std::vector<Entry*> items;
};

Entry* dbFind(Database& db, const string& name);
bool dbLoad(Database& db, const string& path);
bool dbSave(const Database& db, const string& path);
bool dbSaveText(const Database& db, const string& path);
void dbFree(Database& db);


bool dbExecute(Database& db, const std::vector<string>& cmd, bool& changed);