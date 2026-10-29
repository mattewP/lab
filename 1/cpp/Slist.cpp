#include "slist.h"
#include <iostream>

// добавить в голову
void slPushHead(SList& l, const string& v) {
    SNode* nd = new SNode{v, l.head};
    l.head = nd;
    if (!l.tail) l.tail = nd;
    l.n++;
}

// добавить в хвост
void slPushTail(SList& l, const string& v) {
    SNode* nd = new SNode{v, nullptr};
    if (l.tail) l.tail->next = nd; else l.head = nd;
    l.tail = nd;
    l.n++;
}

// добавить после
bool slPushAfter(SList& l, const string& target, const string& v) {
    for (SNode* c = l.head; c; c = c->next)
        if (c->val == target) {
            c->next = new SNode{v, c->next};
            if (c == l.tail) l.tail = c->next;
            l.n++;
            return true;
        }
    return false;
}

// добавить перед
bool slPushBefore(SList& l, const string& target, const string& v) {
    if (!l.head) return false;
    if (l.head->val == target) { slPushHead(l, v); return true; }
    for (SNode* c = l.head; c->next; c = c->next)
        if (c->next->val == target) {
            c->next = new SNode{v, c->next};
            l.n++;
            return true;
        }
    return false;
}

// удалить голову
bool slDelHead(SList& l) {
    if (!l.head) return false;
    SNode* t = l.head;
    l.head = t->next;
    if (!l.head) l.tail = nullptr;
    delete t;
    l.n--;
    return true;
}

// удалить хвост
bool slDelTail(SList& l) {
    if (!l.head) return false;
    if (l.head == l.tail) return slDelHead(l);
    SNode* c = l.head;
    while (c->next != l.tail) c = c->next;
    delete l.tail;
    l.tail = c;
    c->next = nullptr;
    l.n--;
    return true;
}

// удалить элемент после
bool slDelAfter(SList& l, const string& target) {
    for (SNode* c = l.head; c; c = c->next)
        if (c->val == target) {
            SNode* t = c->next;
            if (!t) return false;
            c->next = t->next;
            if (t == l.tail) l.tail = c;
            delete t;
            l.n--;
            return true;
        }
    return false;
}

// удалить элемент перед
bool slDelBefore(SList& l, const string& target) {
    if (!l.head || l.head->val == target) return false;
    if (l.head->next && l.head->next->val == target) return slDelHead(l);
    for (SNode* c = l.head; c->next && c->next->next; c = c->next)
        if (c->next->next->val == target) {
            SNode* t = c->next;
            c->next = t->next;
            delete t;
            l.n--;
            return true;
        }
    return false;
}

// удалить первое вхождение значения
bool slDelValue(SList& l, const string& v) {
    if (!l.head) return false;
    if (l.head->val == v) return slDelHead(l);
    for (SNode* c = l.head; c->next; c = c->next)
        if (c->next->val == v) {
            SNode* t = c->next;
            c->next = t->next;
            if (t == l.tail) l.tail = c;
            delete t;
            l.n--;
            return true;
        }
    return false;
}

// индекс первого вхождения или -1
int slFind(const SList& l, const string& v) {
    int i = 0;
    for (SNode* c = l.head; c; c = c->next, i++)
        if (c->val == v) return i;
    return -1;
}

// прочитать по индексу
bool slGet(const SList& l, size_t idx, string& out) {
    if (idx >= l.n) return false;
    SNode* c = l.head;
    while (idx--) c = c->next;
    out = c->val;
    return true;
}

bool slFront(const SList& l, string& out) { if (!l.head) return false; out = l.head->val; return true; }
bool slBack(const SList& l, string& out) { if (!l.tail) return false; out = l.tail->val; return true; }

void slPrint(const SList& l) {
    for (SNode* c = l.head; c; c = c->next) std::cout << c->val << " -> ";
    std::cout << "null\n";
}

void slFree(SList& l) {
    while (slDelHead(l)) {}
}