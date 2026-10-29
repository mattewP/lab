#include "array.h"
#include <iostream>

// удвоение ёмкости
void arrGrow(Array& a) {
    size_t ncap = a.cap ? a.cap * 2 : 4;
    string* nd = new string[ncap];
    for (size_t i = 0; i < a.len; i++) nd[i] = a.data[i];
    delete[] a.data;
    a.data = nd;
    a.cap = ncap;
}

// добавить в конец
void arrPushBack(Array& a, const string& v) {
    if (a.len == a.cap) arrGrow(a);
    a.data[a.len++] = v;
}

// вставка по индексу
bool arrInsert(Array& a, size_t idx, const string& v) {
    if (idx > a.len) return false;
    if (a.len == a.cap) arrGrow(a);
    for (size_t i = a.len; i > idx; i--) a.data[i] = a.data[i - 1];
    a.data[idx] = v;
    a.len++;
    return true;
}

// получить по индексу
bool arrGet(const Array& a, size_t idx, string& out) {
    if (idx >= a.len) return false;
    out = a.data[idx];
    return true;
}

// удалить по индексу
bool arrDel(Array& a, size_t idx) {
    if (idx >= a.len) return false;
    for (size_t i = idx; i + 1 < a.len; i++) a.data[i] = a.data[i + 1];
    a.len--;
    return true;
}

// заменить по индексу
bool arrSet(Array& a, size_t idx, const string& v) {
    if (idx >= a.len) return false;
    a.data[idx] = v;
    return true;
}

void arrPrint(const Array& a) {
    std::cout << "[";
    for (size_t i = 0; i < a.len; i++) std::cout << (i ? ", " : "") << a.data[i];
    std::cout << "]\n";
}

void arrFree(Array& a) {
    delete[] a.data;
    a.data = nullptr;
    a.len = a.cap = 0;
}