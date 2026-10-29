
#ifndef LAB1_ARRAY_H
#define LAB1_ARRAY_H
#pragma once
#include <string>

using std::string;

struct Array {
    string* data = nullptr;
    size_t len = 0;
    size_t cap = 0;
};

void arrGrow(Array& a);
void arrPushBack(Array& a, const string& v);
bool arrInsert(Array& a, size_t idx, const string& v);
bool arrGet(const Array& a, size_t idx, string& out);
bool arrDel(Array& a, size_t idx);
bool arrSet(Array& a, size_t idx, const string& v);
void arrPrint(const Array& a);
void arrFree(Array& a);
#endif //LAB1_ARRAY_H