package main

import (
	"fmt"
	"io"
	"os"
	"strconv"
	"strings"
)

// Entry - именованная структура в базе
type Entry struct {
	typ  byte
	name string
	arr  Array
	sl   SList
	dl   DList
	st   Stack
	qu   Queue
}

// Database - все структуры из файла
type Database struct {
	items []*Entry
}

// ---------- вспомогательное ----------

func fail(msg string) bool {
	fmt.Fprintln(os.Stderr, "ERROR: "+msg)
	return false
}

// проверка числа аргументов (вместе с именем команды)
func argc(c []string, n int) bool {
	if len(c) != n {
		return fail("wrong number of arguments for " + c[0])
	}
	return true
}

func toIndex(s string) (int, bool) {
	if len(s) == 0 || len(s) > 9 {
		return 0, false
	}
	for _, ch := range s {
		if ch < '0' || ch > '9' {
			return 0, false
		}
	}
	n, err := strconv.Atoi(s)
	return n, err == nil
}

// команды, которые создают структуру, если её ещё нет
func creates(op string) bool {
	switch op {
	case "MPUSH", "MINSERT", "FPUSH", "FPUSHHEAD", "FPUSHTAIL",
		"LPUSH", "LPUSHHEAD", "LPUSHTAIL", "SPUSH", "QPUSH":
		return true
	}
	return false
}

// список всех известных команд
var knownOps = map[string]bool{
	"PRINT": true,
	"MPUSH": true, "MINSERT": true, "MGET": true, "MSET": true, "MDEL": true, "MLEN": true,
	"FPUSH": true, "FPUSHHEAD": true, "FPUSHTAIL": true, "FPUSHBEFORE": true, "FPUSHAFTER": true,
	"FDELHEAD": true, "FDELTAIL": true, "FDELBEFORE": true, "FDELAFTER": true, "FDEL": true,
	"FGET": true, "FHEAD": true, "FTAIL": true, "FFIND": true, "FLEN": true,
	"LPUSH": true, "LPUSHHEAD": true, "LPUSHTAIL": true, "LPUSHBEFORE": true, "LPUSHAFTER": true,
	"LDELHEAD": true, "LDELTAIL": true, "LDELBEFORE": true, "LDELAFTER": true, "LDEL": true,
	"LGET": true, "LHEAD": true, "LTAIL": true, "LFIND": true, "LPRINTR": true, "LLEN": true,
	"SPUSH": true, "SPOP": true, "SPEEK": true, "SLEN": true,
	"QPUSH": true, "QPOP": true, "QPEEK": true, "QLEN": true,
}

func dbFind(db *Database, name string) *Entry {
	for _, e := range db.items {
		if e.name == name {
			return e
		}
	}
	return nil
}

// ---------- файл данных ----------

func dbSave(db *Database, path string) bool {
	var b strings.Builder
	for _, e := range db.items {
		b.WriteByte(e.typ)
		b.WriteByte(' ')
		b.WriteString(e.name)
		switch e.typ {
		case 'M':
			for i := 0; i < e.arr.n; i++ {
				b.WriteString(" " + e.arr.data[i])
			}
		case 'F':
			for c := e.sl.head; c != nil; c = c.next {
				b.WriteString(" " + c.val)
			}
		case 'L':
			for c := e.dl.head; c != nil; c = c.next {
				b.WriteString(" " + c.val)
			}
		case 'S':
			for c := e.st.top; c != nil; c = c.next {
				b.WriteString(" " + c.val)
			}
		case 'Q':
			for c := e.qu.head; c != nil; c = c.next {
				b.WriteString(" " + c.val)
			}
		}
		b.WriteByte('\n')
	}
	return os.WriteFile(path, []byte(b.String()), 0644) == nil
}

func dbLoad(db *Database, path string) bool {
	data, err := os.ReadFile(path)
	if err != nil {
		if os.IsNotExist(err) {
			return true // файла нет: начинаем с пустой базы
		}
		return fail("cannot read file " + path)
	}
	for _, line := range strings.Split(string(data), "\n") {
		tok := strings.Fields(line)
		if len(tok) < 2 {
			continue
		}
		e := &Entry{typ: tok[0][0], name: tok[1]}
		db.items = append(db.items, e)
		vals := tok[2:]
		switch e.typ {
		case 'M':
			for _, v := range vals {
				e.arr.PushBack(v)
			}
		case 'F':
			for _, v := range vals {
				e.sl.PushTail(v)
			}
		case 'L':
			for _, v := range vals {
				e.dl.PushTail(v)
			}
		case 'S':
			// в файле вершина стека записана первой, поэтому кладём с конца
			for i := len(vals) - 1; i >= 0; i-- {
				e.st.Push(vals[i])
			}
		case 'Q':
			for _, v := range vals {
				e.qu.Push(v)
			}
		default:
			return fail("bad file format: " + line)
		}
	}
	return true
}

// ---------- вывод ----------

func printEntry(w io.Writer, e *Entry) {
	switch e.typ {
	case 'M':
		e.arr.Print(w)
	case 'F':
		e.sl.Print(w)
	case 'L':
		e.dl.Print(w)
	case 'S':
		e.st.Print(w)
	case 'Q':
		e.qu.Print(w)
	}
}

func typeName(t byte) string {
	switch t {
	case 'M':
		return "массив"
	case 'F':
		return "односвязный список"
	case 'L':
		return "двусвязный список"
	case 'S':
		return "стек"
	case 'Q':
		return "очередь"
	}
	return "неизвестный тип"
}

// читаемый txt-файл
func dbSaveText(db *Database, path string) bool {
	var b strings.Builder
	for _, e := range db.items {
		fmt.Fprintf(&b, "=== %s (%s) ===\n", e.name, typeName(e.typ))
		printEntry(&b, e)
		b.WriteString("\n")
	}
	return os.WriteFile(path, []byte(b.String()), 0644) == nil
}

// ---------- команды по типам ----------

func doArray(e *Entry, c []string, op string, changed *bool) bool {
	a := &e.arr
	switch op {
	case "MPUSH":
		if !argc(c, 3) {
			return false
		}
		a.PushBack(c[2])
		*changed = true
		fmt.Println(c[2])
	case "MINSERT":
		if !argc(c, 4) {
			return false
		}
		i, ok := toIndex(c[2])
		if !ok || !a.Insert(i, c[3]) {
			return fail("bad index")
		}
		*changed = true
		fmt.Println(c[3])
	case "MGET":
		if !argc(c, 3) {
			return false
		}
		i, ok := toIndex(c[2])
		var s string
		if ok {
			s, ok = a.Get(i)
		}
		if !ok {
			return fail("bad index")
		}
		fmt.Println(s)
	case "MSET":
		if !argc(c, 4) {
			return false
		}
		i, ok := toIndex(c[2])
		if !ok || !a.Set(i, c[3]) {
			return fail("bad index")
		}
		*changed = true
		fmt.Println(c[3])
	case "MDEL":
		if !argc(c, 3) {
			return false
		}
		i, ok := toIndex(c[2])
		if !ok || !a.Del(i) {
			return fail("bad index")
		}
		*changed = true
		fmt.Println("OK")
	case "MLEN":
		if !argc(c, 2) {
			return false
		}
		fmt.Println(a.n)
	default:
		return fail("unknown command " + c[0])
	}
	return true
}

func okOrNotFound(ok bool) {
	if ok {
		fmt.Println("OK")
	} else {
		fmt.Println("NOT FOUND")
	}
}

func doSList(e *Entry, c []string, op string, changed *bool) bool {
	l := &e.sl
	switch op {
	case "FPUSH", "FPUSHTAIL":
		if !argc(c, 3) {
			return false
		}
		l.PushTail(c[2])
		*changed = true
		fmt.Println(c[2])
	case "FPUSHHEAD":
		if !argc(c, 3) {
			return false
		}
		l.PushHead(c[2])
		*changed = true
		fmt.Println(c[2])
	case "FPUSHBEFORE":
		if !argc(c, 4) {
			return false
		}
		if l.PushBefore(c[2], c[3]) {
			*changed = true
			fmt.Println(c[3])
		} else {
			fmt.Println("NOT FOUND")
		}
	case "FPUSHAFTER":
		if !argc(c, 4) {
			return false
		}
		if l.PushAfter(c[2], c[3]) {
			*changed = true
			fmt.Println(c[3])
		} else {
			fmt.Println("NOT FOUND")
		}
	case "FDELHEAD", "FDELTAIL":
		if !argc(c, 2) {
			return false
		}
		var ok bool
		if op == "FDELHEAD" {
			ok = l.DelHead()
		} else {
			ok = l.DelTail()
		}
		if ok {
			*changed = true
		}
		okOrNotFound(ok)
	case "FDELBEFORE", "FDELAFTER", "FDEL":
		if !argc(c, 3) {
			return false
		}
		var ok bool
		switch op {
		case "FDELBEFORE":
			ok = l.DelBefore(c[2])
		case "FDELAFTER":
			ok = l.DelAfter(c[2])
		default:
			ok = l.DelValue(c[2])
		}
		if ok {
			*changed = true
		}
		okOrNotFound(ok)
	case "FGET":
		if !argc(c, 3) {
			return false
		}
		i, ok := toIndex(c[2])
		var s string
		if ok {
			s, ok = l.Get(i)
		}
		if !ok {
			return fail("bad index")
		}
		fmt.Println(s)
	case "FHEAD", "FTAIL":
		if !argc(c, 2) {
			return false
		}
		var s string
		var ok bool
		if op == "FHEAD" {
			s, ok = l.Front()
		} else {
			s, ok = l.Back()
		}
		if ok {
			fmt.Println(s)
		} else {
			fmt.Println("EMPTY")
		}
	case "FFIND":
		if !argc(c, 3) {
			return false
		}
		fmt.Println(l.Find(c[2]))
	case "FLEN":
		if !argc(c, 2) {
			return false
		}
		fmt.Println(l.n)
	default:
		return fail("unknown command " + c[0])
	}
	return true
}

func doDList(e *Entry, c []string, op string, changed *bool) bool {
	l := &e.dl
	switch op {
	case "LPUSH", "LPUSHTAIL":
		if !argc(c, 3) {
			return false
		}
		l.PushTail(c[2])
		*changed = true
		fmt.Println(c[2])
	case "LPUSHHEAD":
		if !argc(c, 3) {
			return false
		}
		l.PushHead(c[2])
		*changed = true
		fmt.Println(c[2])
	case "LPUSHBEFORE":
		if !argc(c, 4) {
			return false
		}
		if l.PushBefore(c[2], c[3]) {
			*changed = true
			fmt.Println(c[3])
		} else {
			fmt.Println("NOT FOUND")
		}
	case "LPUSHAFTER":
		if !argc(c, 4) {
			return false
		}
		if l.PushAfter(c[2], c[3]) {
			*changed = true
			fmt.Println(c[3])
		} else {
			fmt.Println("NOT FOUND")
		}
	case "LDELHEAD", "LDELTAIL":
		if !argc(c, 2) {
			return false
		}
		var ok bool
		if op == "LDELHEAD" {
			ok = l.DelHead()
		} else {
			ok = l.DelTail()
		}
		if ok {
			*changed = true
		}
		okOrNotFound(ok)
	case "LDELBEFORE", "LDELAFTER", "LDEL":
		if !argc(c, 3) {
			return false
		}
		var ok bool
		switch op {
		case "LDELBEFORE":
			ok = l.DelBefore(c[2])
		case "LDELAFTER":
			ok = l.DelAfter(c[2])
		default:
			ok = l.DelValue(c[2])
		}
		if ok {
			*changed = true
		}
		okOrNotFound(ok)
	case "LGET":
		if !argc(c, 3) {
			return false
		}
		i, ok := toIndex(c[2])
		var s string
		if ok {
			s, ok = l.Get(i)
		}
		if !ok {
			return fail("bad index")
		}
		fmt.Println(s)
	case "LHEAD", "LTAIL":
		if !argc(c, 2) {
			return false
		}
		var s string
		var ok bool
		if op == "LHEAD" {
			s, ok = l.Front()
		} else {
			s, ok = l.Back()
		}
		if ok {
			fmt.Println(s)
		} else {
			fmt.Println("EMPTY")
		}
	case "LFIND":
		if !argc(c, 3) {
			return false
		}
		fmt.Println(l.Find(c[2]))
	case "LPRINTR":
		if !argc(c, 2) {
			return false
		}
		l.PrintReverse(os.Stdout)
	case "LLEN":
		if !argc(c, 2) {
			return false
		}
		fmt.Println(l.n)
	default:
		return fail("unknown command " + c[0])
	}
	return true
}

func doStack(e *Entry, c []string, op string, changed *bool) bool {
	switch op {
	case "SPUSH":
		if !argc(c, 3) {
			return false
		}
		e.st.Push(c[2])
		*changed = true
		fmt.Println(c[2])
	case "SPOP":
		if !argc(c, 2) {
			return false
		}
		if s, ok := e.st.Pop(); ok {
			*changed = true
			fmt.Println(s)
		} else {
			fmt.Println("EMPTY")
		}
	case "SPEEK":
		if !argc(c, 2) {
			return false
		}
		if s, ok := e.st.Peek(); ok {
			fmt.Println(s)
		} else {
			fmt.Println("EMPTY")
		}
	case "SLEN":
		if !argc(c, 2) {
			return false
		}
		fmt.Println(e.st.n)
	default:
		return fail("unknown command " + c[0])
	}
	return true
}

func doQueue(e *Entry, c []string, op string, changed *bool) bool {
	switch op {
	case "QPUSH":
		if !argc(c, 3) {
			return false
		}
		e.qu.Push(c[2])
		*changed = true
		fmt.Println(c[2])
	case "QPOP":
		if !argc(c, 2) {
			return false
		}
		if s, ok := e.qu.Pop(); ok {
			*changed = true
			fmt.Println(s)
		} else {
			fmt.Println("EMPTY")
		}
	case "QPEEK":
		if !argc(c, 2) {
			return false
		}
		if s, ok := e.qu.Peek(); ok {
			fmt.Println(s)
		} else {
			fmt.Println("EMPTY")
		}
	case "QLEN":
		if !argc(c, 2) {
			return false
		}
		fmt.Println(e.qu.n)
	default:
		return fail("unknown command " + c[0])
	}
	return true
}

// ---------- диспетчер ----------

func dbExecute(db *Database, cmd []string, changed *bool) bool {
	*changed = false
	if len(cmd) == 0 {
		return fail("empty query")
	}
	op := strings.ToUpper(cmd[0])
	if !knownOps[op] {
		return fail("unknown command " + cmd[0])
	}
	if len(cmd) < 2 {
		return fail("missing structure name")
	}
	name := cmd[1]

	if op == "PRINT" {
		e := dbFind(db, name)
		if e == nil {
			return fail("structure not found: " + name)
		}
		printEntry(os.Stdout, e)
		return true
	}

	typ := op[0]
	e := dbFind(db, name)
	if e == nil {
		if !creates(op) {
			return fail("structure not found: " + name)
		}
		e = &Entry{typ: typ, name: name}
		db.items = append(db.items, e)
		*changed = true
	} else if e.typ != typ {
		return fail(name + " has another type")
	}

	switch typ {
	case 'M':
		return doArray(e, cmd, op, changed)
	case 'F':
		return doSList(e, cmd, op, changed)
	case 'L':
		return doDList(e, cmd, op, changed)
	case 'S':
		return doStack(e, cmd, op, changed)
	default:
		return doQueue(e, cmd, op, changed)
	}
}
