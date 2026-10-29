#include "queue.h"
#include <iostream>

void quPush(Queue& q, const string& v) {
    SNode* nd = new SNode{v, nullptr};
    if (q.tail) q.tail->next = nd; else q.head = nd;
    q.tail = nd;
    q.n++;
}

bool quPop(Queue& q, string& out) {
    if (!q.head) return false;
    SNode* t = q.head;
    out = t->val;
    q.head = t->next;
    if (!q.head) q.tail = nullptr;
    delete t;
    q.n--;
    return true;
}

bool quPeek(const Queue& q, string& out) {
    if (!q.head) return false;
    out = q.head->val;
    return true;
}

void quPrint(const Queue& q) {
    std::cout << "front -> ";
    for (SNode* c = q.head; c; c = c->next) std::cout << c->val << " ";
    std::cout << "<- back\n";
}

void quFree(Queue& q) {
    string t;
    while (quPop(q, t)) {}
}