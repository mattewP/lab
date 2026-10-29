
#ifndef LAB1_QUEUE_H
#define LAB1_QUEUE_H
#pragma once
#include <string>
#include "slist.h"

using std::string;

struct Queue {
    SNode* head = nullptr;
    SNode* tail = nullptr;
    size_t n = 0;
};

void quPush(Queue& q, const string& v);
bool quPop(Queue& q, string& out);
bool quPeek(const Queue& q, string& out);
void quPrint(const Queue& q);
void quFree(Queue& q);
#endif //LAB1_QUEUE_H