#include "Stack.h"
#include <iostream>

void stPush(Stack& s, const string& v) {
    s.top = new SNode{v, s.top};
    s.n++;
}

// взять и удалить
bool stPop(Stack& s, string& out) {
    if (!s.top) return false;
    SNode* t = s.top;
    out = t->val;
    s.top = t->next;
    delete t;
    s.n--;
    return true;
    //модифицировать чтобы то что  я удалил выводило в консоль и в оцереди

}

// прочитать без удаления
bool stPeek(const Stack& s, string& out) {
    if (!s.top) return false;
    out = s.top->val;
    return true;
}

void stPrint(const Stack& s) {
    std::cout << "top -> ";
    for (SNode* c = s.top; c; c = c->next) std::cout << c->val << " ";
    std::cout << "\n";
}

void stFree(Stack& s) {
    string t;
    while (stPop(s, t)) {}
}