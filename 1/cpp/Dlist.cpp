#include "dlist.h"
#include <iostream>

// поиск узла по значению
DNode* dlFindNode(const DList& l, const string& v) {
    for (DNode* c = l.head; c; c = c->next)
        if (c->val == v) return c;
    return nullptr;
}

// отцепить и удалить узел
void dlUnlink(DList& l, DNode* c) {
    if (c->prev) c->prev->next = c->next; else l.head = c->next;
    if (c->next) c->next->prev = c->prev; else l.tail = c->prev;
    delete c;
    l.n--;
}

void dlPushHead(DList& l, const string& v) {
    DNode* nd = new DNode{v, nullptr, l.head};
    if (l.head) l.head->prev = nd; else l.tail = nd;
    l.head = nd;
    l.n++;
}

void dlPushTail(DList& l, const string& v) {
    DNode* nd = new DNode{v, l.tail, nullptr};
    if (l.tail) l.tail->next = nd; else l.head = nd;
    l.tail = nd;
    l.n++;
}

bool dlPushAfter(DList& l, const string& target, const string& v) {
    DNode* c = dlFindNode(l, target);
    if (!c) return false;
    DNode* nd = new DNode{v, c, c->next};
    if (c->next) c->next->prev = nd; else l.tail = nd;
    c->next = nd;
    l.n++;
    return true;
}

bool dlPushBefore(DList& l, const string& target, const string& v) {
    DNode* c = dlFindNode(l, target);
    if (!c) return false;
    DNode* nd = new DNode{v, c->prev, c};
    if (c->prev) c->prev->next = nd; else l.head = nd;
    c->prev = nd;
    l.n++;
    return true;
}

bool dlDelHead(DList& l) { if (!l.head) return false; dlUnlink(l, l.head); return true; }
bool dlDelTail(DList& l) { if (!l.tail) return false; dlUnlink(l, l.tail); return true; }

bool dlDelAfter(DList& l, const string& target) {
    DNode* c = dlFindNode(l, target);
    if (!c || !c->next) return false;
    dlUnlink(l, c->next);
    return true;
}

bool dlDelBefore(DList& l, const string& target) {
    DNode* c = dlFindNode(l, target);
    if (!c || !c->prev) return false;
    dlUnlink(l, c->prev);
    return true;
}

bool dlDelValue(DList& l, const string& v) {
    DNode* c = dlFindNode(l, v);
    if (!c) return false;
    dlUnlink(l, c);
    return true;
}

int dlFind(const DList& l, const string& v) {
    int i = 0;
    for (DNode* c = l.head; c; c = c->next, i++)
        if (c->val == v) return i;
    return -1;
}

// чтение по индексу с ближнего конца
bool dlGet(const DList& l, size_t idx, string& out) {
    if (idx >= l.n) return false;
    DNode* c;
    if (idx < l.n / 2) { c = l.head; while (idx--) c = c->next; }
    else { c = l.tail; idx = l.n - 1 - idx; while (idx--) c = c->prev; }
    out = c->val;
    return true;
}

bool dlFront(const DList& l, string& out) { if (!l.head) return false; out = l.head->val; return true; }
bool dlBack(const DList& l, string& out) { if (!l.tail) return false; out = l.tail->val; return true; }

void dlPrint(const DList& l) {
    std::cout << "null <-> ";
    for (DNode* c = l.head; c; c = c->next) std::cout << c->val << " <-> ";
    std::cout << "null\n";
}

// вывод с хвоста
void dlPrintReverse(const DList& l) {
    std::cout << "null <-> ";
    for (DNode* c = l.tail; c; c = c->prev) std::cout << c->val << " <-> ";
    std::cout << "null\n";
}

void dlFree(DList& l) {
    while (l.head) dlUnlink(l, l.head);
}