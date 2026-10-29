#ifndef LAB1_STECK_H
#define LAB1_STECK_H
#pragma once
#include <string>
#include "slist.h"

using std::string;

struct Stack {
    SNode* top = nullptr;
    size_t n = 0;
};

void stPush(Stack& s, const string& v);
bool stPop(Stack& s, string& out);
bool stPeek(const Stack& s, string& out);
void stPrint(const Stack& s);
void stFree(Stack& s);
#endif //LAB1_STECK_H