#include "tree.h"
#include <iostream>
#include <cstdlib>
#include <queue>
#include <vector>

// сравнение значений: числа сравниваются как числа и идут раньше строк, строки сравниваются лексикографически
static bool toNum(const string& s, double& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    out = std::strtod(s.c_str(), &end);
    return *end == '\0';
}

static int cmpVal(const string& a, const string& b) {
    double x, y;
    bool na = toNum(a, x), nb = toNum(b, y);
    if (na && nb) return x < y ? -1 : (x > y ? 1 : 0);
    if (na != nb) return na ? -1 : 1;
    return a < b ? -1 : (a > b ? 1 : 0);
}

// поиск по значению: меньше идём влево, больше вправо
TNode* btFindNode(TNode* root, const string& v) {
    TNode* c = root;
    while (c) {
        int r = cmpVal(v, c->val);
        if (r == 0) return c;
        c = r < 0 ? c->left : c->right;
    }
    return nullptr;
}

bool btFind(TNode* root, const string& v) { return btFindNode(root, v) != nullptr; }

// вставка: программа сама выбирает место, меньшее значение слева, большее справа
// возвращает false, если такое значение уже есть
bool btInsert(TNode*& root, const string& v) {
    TNode** cur = &root;
    while (*cur) {
        int r = cmpVal(v, (*cur)->val);
        if (r == 0) return false;
        cur = r < 0 ? &(*cur)->left : &(*cur)->right;
    }
    *cur = new TNode{v, nullptr, nullptr};
    return true;
}

// удаление с сохранением структуры дерева поиска; false, если значения нет
bool btDelete(TNode*& root, const string& v) {
    TNode** cur = &root;
    while (*cur) {
        int r = cmpVal(v, (*cur)->val);
        if (r == 0) break;
        cur = r < 0 ? &(*cur)->left : &(*cur)->right;
    }
    if (!*cur) return false;
    TNode* d = *cur;
    if (!d->left || !d->right) {
        *cur = d->left ? d->left : d->right;
        delete d;
    } else {
        TNode** s = &d->right;
        while ((*s)->left) s = &(*s)->left;
        TNode* succ = *s;
        d->val = succ->val;
        *s = succ->right;
        delete succ;
    }
    return true;
}

// проверка на Full рекурсивно
bool btIsFull(const TNode* x) {
    if (!x) return true;
    if (!x->left && !x->right) return true;
    if (x->left && x->right) return btIsFull(x->left) && btIsFull(x->right);
    return false;
}

// обходы: прямой, симметричный, обратный
void btPre(const TNode* x) { if (!x) return; std::cout << x->val << " "; btPre(x->left); btPre(x->right); }
void btIn(const TNode* x) { if (!x) return; btIn(x->left); std::cout << x->val << " "; btIn(x->right); }
void btPost(const TNode* x) { if (!x) return; btPost(x->left); btPost(x->right); std::cout << x->val << " "; }

// обход в ширину
void btLevel(TNode* root) {
    if (!root) return;
    std::queue<TNode*> q;
    q.push(root);
    while (!q.empty()) {
        TNode* c = q.front(); q.pop();
        std::cout << c->val << " ";
        if (c->left) q.push(c->left);
        if (c->right) q.push(c->right);
    }
}

// вывод дерева сверху вниз
static std::vector<string> glyphs(const string& s) {
    std::vector<string> r;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = (unsigned char)s[i];
        size_t n = c < 0x80 ? 1 : (c >> 5) == 6 ? 2 : (c >> 4) == 14 ? 3 : 4;
        r.push_back(s.substr(i, n));
        i += n;
    }
    return r;
}

// число узлов, высота и самая длинная подпись
static void btMeasure(const TNode* x, size_t depth, size_t& cnt, size_t& height, size_t& maxLen) {
    if (!x) return;
    cnt++;
    if (depth + 1 > height) height = depth + 1;
    size_t len = glyphs(x->val).size();
    if (len > maxLen) maxLen = len;
    btMeasure(x->left, depth + 1, cnt, height, maxLen);
    btMeasure(x->right, depth + 1, cnt, height, maxLen);
}

typedef std::vector<std::vector<string>> Grid;
static const size_t NPOS = (size_t)-1;

// колонка узла = его номер в симметричном обходе, поэтому родитель всегда между детьми
static size_t btPlace(const TNode* x, size_t depth, size_t& idx, size_t W, Grid& g) {
    size_t cl = NPOS, cr = NPOS;
    if (x->left) cl = btPlace(x->left, depth + 1, idx, W, g);
    size_t col = idx * W + W / 2;
    idx++;
    if (x->right) cr = btPlace(x->right, depth + 1, idx, W, g);

    std::vector<string> gl = glyphs(x->val);
    size_t start = col - gl.size() / 2;
    for (size_t i = 0; i < gl.size(); i++) g[2 * depth][start + i] = gl[i];

    if (cl != NPOS || cr != NPOS) {
        size_t lo = cl != NPOS ? cl : col;
        size_t hi = cr != NPOS ? cr : col;
        std::vector<string>& row = g[2 * depth + 1];
        for (size_t c = lo; c <= hi; c++) row[c] = "\u2500";
        if (cl != NPOS) row[cl] = "\u250C";
        if (cr != NPOS) row[cr] = "\u2510";
        row[col] = (cl != NPOS && cr != NPOS) ? "\u2534" : (cl != NPOS ? "\u2518" : "\u2514");
    }
    return col;
}

void btPrint(const TNode* root) {
    if (!root) { std::cout << "(empty)\n"; return; }
    size_t cnt = 0, height = 0, maxLen = 0;
    btMeasure(root, 0, cnt, height, maxLen);
    size_t W = maxLen + 2;
    Grid g(2 * height - 1, std::vector<string>(cnt * W, " "));
    size_t idx = 0;
    btPlace(root, 0, idx, W, g);
    for (auto& row : g) {
        size_t end = row.size();
        while (end > 0 && row[end - 1] == " ") end--;
        for (size_t i = 0; i < end; i++) std::cout << row[i];
        std::cout << "\n";
    }
}

void btFree(TNode*& x) {
    if (!x) return;
    btFree(x->left);
    btFree(x->right);
    delete x;
    x = nullptr;
}