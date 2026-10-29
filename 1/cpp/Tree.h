#ifndef LAB1_TREE_H
#define LAB1_TREE_H
#pragma once
#include <string>

using std::string;

struct TNode {
    string val;
    TNode* left;
    TNode* right;
};

TNode* btFindNode(TNode* root, const string& v);
bool btFind(TNode* root, const string& v);
bool btInsert(TNode*& root, const string& v);
bool btDelete(TNode*& root, const string& v);
bool btIsFull(const TNode* x);
void btPre(const TNode* x);
void btIn(const TNode* x);
void btPost(const TNode* x);
void btLevel(TNode* root);
void btPrint(const TNode* root);
void btFree(TNode*& x);
#endif //LAB1_TREE_H