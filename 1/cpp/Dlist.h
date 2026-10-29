
#ifndef LAB1_DLIST_H
#define LAB1_DLIST_H
#pragma once
#include <string>

using std::string;

struct DNode {
    string val;
    DNode* prev;
    DNode* next;
};

struct DList {
    DNode* head = nullptr;
    DNode* tail = nullptr;
    size_t n = 0;
};

DNode* dlFindNode(const DList& l, const string& v);
void dlUnlink(DList& l, DNode* c);
void dlPushHead(DList& l, const string& v);
void dlPushTail(DList& l, const string& v);
bool dlPushAfter(DList& l, const string& target, const string& v);
bool dlPushBefore(DList& l, const string& target, const string& v);
bool dlDelHead(DList& l);
bool dlDelTail(DList& l);
bool dlDelAfter(DList& l, const string& target);
bool dlDelBefore(DList& l, const string& target);
bool dlDelValue(DList& l, const string& v);
int dlFind(const DList& l, const string& v);
bool dlGet(const DList& l, size_t idx, string& out);
bool dlFront(const DList& l, string& out);
bool dlBack(const DList& l, string& out);
void dlPrint(const DList& l);
void dlPrintReverse(const DList& l);
void dlFree(DList& l);
#endif //LAB1_DLIST_H