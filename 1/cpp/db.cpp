#include "db.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

using std::cout;
using std::vector;

// ---------- вспомогательное ----------
static bool fail(const string& msg) {
    std::cerr << "ERROR: " << msg << "\n";
    return false;
}

static string upper(string s) {
    for (char& c : s) c = (char)toupper((unsigned char)c);
    return s;
}

// проверка числа аргументов (вместе с именем команды)
static bool argc(const vector<string>& c, size_t n) {
    if (c.size() != n) return fail("wrong number of arguments for " + c[0]);
    return true;
}

static bool toIndex(const string& s, size_t& out) {
    if (s.empty() || s.size() > 9) return false;
    for (char ch : s) if (!isdigit((unsigned char)ch)) return false;
    out = std::stoul(s);
    return true;
}

// команды, которые создают структуру, если её ещё нет
static bool creates(const string& op) {
    return op == "MPUSH" || op == "MINSERT" || op == "FPUSH" || op == "FPUSHHEAD" || op == "FPUSHTAIL" ||
           op == "LPUSH" || op == "LPUSHHEAD" || op == "LPUSHTAIL" || op == "SPUSH" || op == "QPUSH" ||
           op == "TINSERT";
}

// список всех известных команд
static bool known(const string& op) {
    static const char* all[] = {
        "PRINT", "MPUSH", "MINSERT", "MGET", "MSET", "MDEL", "MLEN",
        "FPUSH", "FPUSHHEAD", "FPUSHTAIL", "FPUSHBEFORE", "FPUSHAFTER", "FDELHEAD", "FDELTAIL",
        "FDELBEFORE", "FDELAFTER", "FDEL", "FGET", "FHEAD", "FTAIL", "FFIND", "FLEN",
        "LPUSH", "LPUSHHEAD", "LPUSHTAIL", "LPUSHBEFORE", "LPUSHAFTER", "LDELHEAD", "LDELTAIL",
        "LDELBEFORE", "LDELAFTER", "LDEL", "LGET", "LHEAD", "LTAIL", "LFIND", "LPRINTR", "LLEN",
        "SPUSH", "SPOP", "SPEEK", "SLEN", "QPUSH", "QPOP", "QPEEK", "QLEN",
        "TINSERT", "TDEL", "TGET", "TISFULL", "TPRE", "TIN", "TPOST", "TBFS"};
    for (const char* c : all)
        if (op == c) return true;
    return false;
}

Entry* dbFind(Database& db, const string& name) {
    for (Entry* e : db.items)
        if (e->name == name) return e;
    return nullptr;
}

void dbFree(Database& db) {
    for (Entry* e : db.items) {
        arrFree(e->arr);
        slFree(e->sl);
        dlFree(e->dl);
        stFree(e->st);
        quFree(e->qu);
        btFree(e->root);
        delete e;
    }
    db.items.clear();
}



static void treeSave(const TNode* x, std::ostream& f) {
    if (!x) { f << " #"; return; }
    f << ' ' << x->val;
    treeSave(x->left, f);
    treeSave(x->right, f);
}

static TNode* treeLoad(const vector<string>& t, size_t& i) {
    if (i >= t.size()) return nullptr;
    if (t[i] == "#") { i++; return nullptr; }
    TNode* nd = new TNode{t[i++], nullptr, nullptr};
    nd->left = treeLoad(t, i);
    nd->right = treeLoad(t, i);
    return nd;
}

bool dbSave(const Database& db, const string& path) {
    std::ofstream f(path);
    if (!f) return false;
    for (const Entry* e : db.items) {
        f << e->type << ' ' << e->name;
        switch (e->type) {
            case 'M':
                for (size_t i = 0; i < e->arr.len; i++) f << ' ' << e->arr.data[i];
                break;
            case 'F':
                for (SNode* c = e->sl.head; c; c = c->next) f << ' ' << c->val;
                break;
            case 'L':
                for (DNode* c = e->dl.head; c; c = c->next) f << ' ' << c->val;
                break;
            case 'S':
                for (SNode* c = e->st.top; c; c = c->next) f << ' ' << c->val;
                break;
            case 'Q':
                for (SNode* c = e->qu.head; c; c = c->next) f << ' ' << c->val;
                break;
            case 'T':
                treeSave(e->root, f);
                break;
        }
        f << '\n';
    }
    return (bool)f;
}

bool dbLoad(Database& db, const string& path) {
    std::ifstream f(path);
    if (!f) return true;  // файла нет: начинаем с пустой базы
    string line;
    while (std::getline(f, line)) {
        std::istringstream in(line);
        vector<string> tok;
        string w;
        while (in >> w) tok.push_back(w);
        if (tok.size() < 2) continue;
        Entry* e = new Entry;
        e->type = tok[0][0];
        e->name = tok[1];
        db.items.push_back(e);
        vector<string> vals(tok.begin() + 2, tok.end());
        switch (e->type) {
            case 'M': for (auto& v : vals) arrPushBack(e->arr, v); break;
            case 'F': for (auto& v : vals) slPushTail(e->sl, v); break;
            case 'L': for (auto& v : vals) dlPushTail(e->dl, v); break;
            case 'S': for (size_t i = vals.size(); i-- > 0;) stPush(e->st, vals[i]); break;
            case 'Q': for (auto& v : vals) quPush(e->qu, v); break;
            case 'T': { size_t i = 0; e->root = treeLoad(vals, i); break; }
            default: return fail("bad file format: " + line);
        }
    }
    return true;
}

// ---------- команды по типам ----------
static bool doArray(Entry* e, const vector<string>& c, const string& op, bool& changed) {
    Array& a = e->arr;
    string s;
    size_t i;
    if (op == "MPUSH") {
        if (!argc(c, 3)) return false;
        arrPushBack(a, c[2]);
        changed = true;
        cout << c[2] << "\n";
    } else if (op == "MINSERT") {
        if (!argc(c, 4)) return false;
        if (!toIndex(c[2], i) || !arrInsert(a, i, c[3])) return fail("bad index");
        changed = true;
        cout << c[3] << "\n";
    } else if (op == "MGET") {
        if (!argc(c, 3)) return false;
        if (!toIndex(c[2], i) || !arrGet(a, i, s)) return fail("bad index");
        cout << s << "\n";
    } else if (op == "MSET") {
        if (!argc(c, 4)) return false;
        if (!toIndex(c[2], i) || !arrSet(a, i, c[3])) return fail("bad index");
        changed = true;
        cout << c[3] << "\n";
    } else if (op == "MDEL") {
        if (!argc(c, 3)) return false;
        if (!toIndex(c[2], i) || !arrDel(a, i)) return fail("bad index");
        changed = true;
        cout << "OK\n";
    } else if (op == "MLEN") {
        if (!argc(c, 2)) return false;
        cout << a.len << "\n";
    } else return fail("unknown command " + c[0]);
    return true;
}

static bool doSList(Entry* e, const vector<string>& c, const string& op, bool& changed) {
    SList& l = e->sl;
    string s;
    size_t i;
    if (op == "FPUSH" || op == "FPUSHTAIL") {
        if (!argc(c, 3)) return false;
        slPushTail(l, c[2]); changed = true; cout << c[2] << "\n";
    } else if (op == "FPUSHHEAD") {
        if (!argc(c, 3)) return false;
        slPushHead(l, c[2]); changed = true; cout << c[2] << "\n";
    } else if (op == "FPUSHBEFORE") {
        if (!argc(c, 4)) return false;
        if (slPushBefore(l, c[2], c[3])) { changed = true; cout << c[3] << "\n"; } else cout << "NOT FOUND\n";
    } else if (op == "FPUSHAFTER") {
        if (!argc(c, 4)) return false;
        if (slPushAfter(l, c[2], c[3])) { changed = true; cout << c[3] << "\n"; } else cout << "NOT FOUND\n";
    } else if (op == "FDELHEAD" || op == "FDELTAIL") {
        if (!argc(c, 2)) return false;
        bool ok = op == "FDELHEAD" ? slDelHead(l) : slDelTail(l);
        if (ok) changed = true;
        cout << (ok ? "OK" : "NOT FOUND") << "\n";
    } else if (op == "FDELBEFORE" || op == "FDELAFTER" || op == "FDEL") {
        if (!argc(c, 3)) return false;
        bool ok = op == "FDELBEFORE" ? slDelBefore(l, c[2]) : op == "FDELAFTER" ? slDelAfter(l, c[2]) : slDelValue(l, c[2]);
        if (ok) changed = true;
        cout << (ok ? "OK" : "NOT FOUND") << "\n";
    } else if (op == "FGET") {
        if (!argc(c, 3)) return false;
        if (!toIndex(c[2], i) || !slGet(l, i, s)) return fail("bad index");
        cout << s << "\n";
    } else if (op == "FHEAD" || op == "FTAIL") {
        if (!argc(c, 2)) return false;
        if (op == "FHEAD" ? slFront(l, s) : slBack(l, s)) cout << s << "\n"; else cout << "EMPTY\n";
    } else if (op == "FFIND") {
        if (!argc(c, 3)) return false;
        cout << slFind(l, c[2]) << "\n";
    } else if (op == "FLEN") {
        if (!argc(c, 2)) return false;
        cout << l.n << "\n";
    } else return fail("unknown command " + c[0]);
    return true;
}

static bool doDList(Entry* e, const vector<string>& c, const string& op, bool& changed) {
    DList& l = e->dl;
    string s;
    size_t i;
    if (op == "LPUSH" || op == "LPUSHTAIL") {
        if (!argc(c, 3)) return false;
        dlPushTail(l, c[2]); changed = true; cout << c[2] << "\n";
    } else if (op == "LPUSHHEAD") {
        if (!argc(c, 3)) return false;
        dlPushHead(l, c[2]); changed = true; cout << c[2] << "\n";
    } else if (op == "LPUSHBEFORE") {
        if (!argc(c, 4)) return false;
        if (dlPushBefore(l, c[2], c[3])) { changed = true; cout << c[3] << "\n"; } else cout << "NOT FOUND\n";
    } else if (op == "LPUSHAFTER") {
        if (!argc(c, 4)) return false;
        if (dlPushAfter(l, c[2], c[3])) { changed = true; cout << c[3] << "\n"; } else cout << "NOT FOUND\n";
    } else if (op == "LDELHEAD" || op == "LDELTAIL") {
        if (!argc(c, 2)) return false;
        bool ok = op == "LDELHEAD" ? dlDelHead(l) : dlDelTail(l);
        if (ok) changed = true;
        cout << (ok ? "OK" : "NOT FOUND") << "\n";
    } else if (op == "LDELBEFORE" || op == "LDELAFTER" || op == "LDEL") {
        if (!argc(c, 3)) return false;
        bool ok = op == "LDELBEFORE" ? dlDelBefore(l, c[2]) : op == "LDELAFTER" ? dlDelAfter(l, c[2]) : dlDelValue(l, c[2]);
        if (ok) changed = true;
        cout << (ok ? "OK" : "NOT FOUND") << "\n";
    } else if (op == "LGET") {
        if (!argc(c, 3)) return false;
        if (!toIndex(c[2], i) || !dlGet(l, i, s)) return fail("bad index");
        cout << s << "\n";
    } else if (op == "LHEAD" || op == "LTAIL") {
        if (!argc(c, 2)) return false;
        if (op == "LHEAD" ? dlFront(l, s) : dlBack(l, s)) cout << s << "\n"; else cout << "EMPTY\n";
    } else if (op == "LFIND") {
        if (!argc(c, 3)) return false;
        cout << dlFind(l, c[2]) << "\n";
    } else if (op == "LPRINTR") {
        if (!argc(c, 2)) return false;
        dlPrintReverse(l);
    } else if (op == "LLEN") {
        if (!argc(c, 2)) return false;
        cout << l.n << "\n";
    } else return fail("unknown command " + c[0]);
    return true;
}

static bool doStack(Entry* e, const vector<string>& c, const string& op, bool& changed) {
    string s;
    if (op == "SPUSH") {
        if (!argc(c, 3)) return false;
        stPush(e->st, c[2]); changed = true; cout << c[2] << "\n";
    } else if (op == "SPOP") {
        if (!argc(c, 2)) return false;
        if (stPop(e->st, s)) { changed = true; cout << s << "\n"; } else cout << "EMPTY\n";
    } else if (op == "SPEEK") {
        if (!argc(c, 2)) return false;
        if (stPeek(e->st, s)) cout << s << "\n"; else cout << "EMPTY\n";
    } else if (op == "SLEN") {
        if (!argc(c, 2)) return false;
        cout << e->st.n << "\n";
    } else return fail("unknown command " + c[0]);
    return true;
}

static bool doQueue(Entry* e, const vector<string>& c, const string& op, bool& changed) {
    string s;
    if (op == "QPUSH") {
        if (!argc(c, 3)) return false;
        quPush(e->qu, c[2]); changed = true; cout << c[2] << "\n";
    } else if (op == "QPOP") {
        if (!argc(c, 2)) return false;
        if (quPop(e->qu, s)) { changed = true; cout << s << "\n"; } else cout << "EMPTY\n";
    } else if (op == "QPEEK") {
        if (!argc(c, 2)) return false;
        if (quPeek(e->qu, s)) cout << s << "\n"; else cout << "EMPTY\n";
    } else if (op == "QLEN") {
        if (!argc(c, 2)) return false;
        cout << e->qu.n << "\n";
    } else return fail("unknown command " + c[0]);
    return true;
}

static bool doTree(Entry* e, const vector<string>& c, const string& op, bool& changed) {
    TNode*& root = e->root;
    if (op == "TINSERT") {
        if (!argc(c, 3)) return false;
        if (c[2] == "#") return fail("value # is reserved");
        if (btInsert(root, c[2])) { changed = true; cout << c[2] << "\n"; } else cout << "EXISTS\n";
    } else if (op == "TDEL") {
        if (!argc(c, 3)) return false;
        bool ok = btDelete(root, c[2]);
        if (ok) changed = true;
        cout << (ok ? "OK" : "NOT FOUND") << "\n";
    } else if (op == "TGET") {
        if (!argc(c, 3)) return false;
        cout << (btFind(root, c[2]) ? "TRUE" : "FALSE") << "\n";
    } else if (op == "TISFULL") {
        if (!argc(c, 2)) return false;
        cout << (btIsFull(root) ? "TRUE" : "FALSE") << "\n";
    } else if (op == "TPRE" || op == "TIN" || op == "TPOST" || op == "TBFS") {
        if (!argc(c, 2)) return false;
        if (op == "TPRE") btPre(root);
        else if (op == "TIN") btIn(root);
        else if (op == "TPOST") btPost(root);
        else btLevel(root);
        cout << "\n";
    } else return fail("unknown command " + c[0]);
    return true;
}

static void printEntry(const Entry* e) {
    switch (e->type) {
        case 'M': arrPrint(e->arr); break;
        case 'F': slPrint(e->sl); break;
        case 'L': dlPrint(e->dl); break;
        case 'S': stPrint(e->st); break;
        case 'Q': quPrint(e->qu); break;
        case 'T': btPrint(e->root); break;
    }
}

// ---------- читаемый txt-файл ----------
static const char* typeName(char t) {
    switch (t) {
        case 'M': return "массив";
        case 'F': return "односвязный список";
        case 'L': return "двусвязный список";
        case 'S': return "стек";
        case 'Q': return "очередь";
        default:  return "бинарное дерево поиска";
    }
}

bool dbSaveText(const Database& db, const string& path) {
    std::ofstream f(path);
    if (!f) return false;
    std::streambuf* old = cout.rdbuf(f.rdbuf());  // print-функции пишут в cout, перенаправляем его в файл
    for (const Entry* e : db.items) {
        cout << "=== " << e->name << " (" << typeName(e->type) << ") ===\n";
        printEntry(e);
        if (e->type == 'T') cout << "isFull: " << (btIsFull(e->root) ? "TRUE" : "FALSE") << "\n";
        cout << "\n";
    }
    cout.rdbuf(old);
    return (bool)f;
}

// ---------- диспетчер ----------
bool dbExecute(Database& db, const vector<string>& cmd, bool& changed) {
    changed = false;
    if (cmd.empty()) return fail("empty query");
    string op = upper(cmd[0]);
    if (op == "ISMEMBER") op = "TGET";
    if (!known(op)) return fail("unknown command " + cmd[0]);
    if (cmd.size() < 2) return fail("missing structure name");
    const string& name = cmd[1];

    if (op == "PRINT") {
        Entry* e = dbFind(db, name);
        if (!e) return fail("structure not found: " + name);
        printEntry(e);
        return true;
    }

    char type = op[0];

    Entry* e = dbFind(db, name);
    if (!e) {
        if (!creates(op)) return fail("structure not found: " + name);
        e = new Entry;
        e->type = type;
        e->name = name;
        db.items.push_back(e);
        changed = true;
    } else if (e->type != type) {
        return fail(name + " has another type");
    }

    switch (type) {
        case 'M': return doArray(e, cmd, op, changed);
        case 'F': return doSList(e, cmd, op, changed);
        case 'L': return doDList(e, cmd, op, changed);
        case 'S': return doStack(e, cmd, op, changed);
        case 'Q': return doQueue(e, cmd, op, changed);
        default:  return doTree(e, cmd, op, changed);
    }
}