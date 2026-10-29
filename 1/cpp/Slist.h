
#ifndef LAB1_SLIST_H
#define LAB1_SLIST_H
#pragma once
#include <string>

using std::string;

struct SNode {
    string val;
    SNode* next;
};

struct SList {
    SNode* head = nullptr;
    SNode* tail = nullptr;
    size_t n = 0;
};

void slPushHead(SList& l, const string& v);
void slPushTail(SList& l, const string& v);
bool slPushAfter(SList& l, const string& target, const string& v);
bool slPushBefore(SList& l, const string& target, const string& v);
bool slDelHead(SList& l);
bool slDelTail(SList& l);
bool slDelAfter(SList& l, const string& target);
bool slDelBefore(SList& l, const string& target);
bool slDelValue(SList& l, const string& v);
int slFind(const SList& l, const string& v);
bool slGet(const SList& l, size_t idx, string& out);
bool slFront(const SList& l, string& out);
bool slBack(const SList& l, string& out);
void slPrint(const SList& l);
void slFree(SList& l);
#endif //LAB1_SLIST_H