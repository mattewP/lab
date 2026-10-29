package main

import (
	"fmt"
	"io"
)

// DNode - узел двусвязного списка
type DNode struct {
	val  string
	prev *DNode
	next *DNode
}

// DList - двусвязный список
type DList struct {
	head *DNode
	tail *DNode
	n    int
}

// findNode - поиск узла по значению
func (l *DList) findNode(v string) *DNode {
	for c := l.head; c != nil; c = c.next {
		if c.val == v {
			return c
		}
	}
	return nil
}

// unlink - отцепить узел от списка
func (l *DList) unlink(c *DNode) {
	if c.prev != nil {
		c.prev.next = c.next
	} else {
		l.head = c.next
	}
	if c.next != nil {
		c.next.prev = c.prev
	} else {
		l.tail = c.prev
	}
	l.n--
}

// PushHead - добавить в голову
func (l *DList) PushHead(v string) {
	nd := &DNode{val: v, next: l.head}
	if l.head != nil {
		l.head.prev = nd
	} else {
		l.tail = nd
	}
	l.head = nd
	l.n++
}

// PushTail - добавить в хвост
func (l *DList) PushTail(v string) {
	nd := &DNode{val: v, prev: l.tail}
	if l.tail != nil {
		l.tail.next = nd
	} else {
		l.head = nd
	}
	l.tail = nd
	l.n++
}

// PushAfter - добавить после target
func (l *DList) PushAfter(target, v string) bool {
	c := l.findNode(target)
	if c == nil {
		return false
	}
	nd := &DNode{val: v, prev: c, next: c.next}
	if c.next != nil {
		c.next.prev = nd
	} else {
		l.tail = nd
	}
	c.next = nd
	l.n++
	return true
}

// PushBefore - добавить перед target
func (l *DList) PushBefore(target, v string) bool {
	c := l.findNode(target)
	if c == nil {
		return false
	}
	nd := &DNode{val: v, prev: c.prev, next: c}
	if c.prev != nil {
		c.prev.next = nd
	} else {
		l.head = nd
	}
	c.prev = nd
	l.n++
	return true
}

// DelHead - удалить голову
func (l *DList) DelHead() bool {
	if l.head == nil {
		return false
	}
	l.unlink(l.head)
	return true
}

// DelTail - удалить хвост
func (l *DList) DelTail() bool {
	if l.tail == nil {
		return false
	}
	l.unlink(l.tail)
	return true
}

// DelAfter - удалить элемент после target
func (l *DList) DelAfter(target string) bool {
	c := l.findNode(target)
	if c == nil || c.next == nil {
		return false
	}
	l.unlink(c.next)
	return true
}

// DelBefore - удалить элемент перед target
func (l *DList) DelBefore(target string) bool {
	c := l.findNode(target)
	if c == nil || c.prev == nil {
		return false
	}
	l.unlink(c.prev)
	return true
}

// DelValue - удалить первое вхождение значения
func (l *DList) DelValue(v string) bool {
	c := l.findNode(v)
	if c == nil {
		return false
	}
	l.unlink(c)
	return true
}

// Find - индекс первого вхождения или -1
func (l *DList) Find(v string) int {
	i := 0
	for c := l.head; c != nil; c = c.next {
		if c.val == v {
			return i
		}
		i++
	}
	return -1
}

// Get - чтение по индексу с ближнего конца
func (l *DList) Get(idx int) (string, bool) {
	if idx < 0 || idx >= l.n {
		return "", false
	}
	var c *DNode
	if idx < l.n/2 {
		c = l.head
		for ; idx > 0; idx-- {
			c = c.next
		}
	} else {
		c = l.tail
		for k := l.n - 1 - idx; k > 0; k-- {
			c = c.prev
		}
	}
	return c.val, true
}

// Front - значение головы
func (l *DList) Front() (string, bool) {
	if l.head == nil {
		return "", false
	}
	return l.head.val, true
}

// Back - значение хвоста
func (l *DList) Back() (string, bool) {
	if l.tail == nil {
		return "", false
	}
	return l.tail.val, true
}

// Print - вывод с головы
func (l *DList) Print(w io.Writer) {
	fmt.Fprint(w, "null <-> ")
	for c := l.head; c != nil; c = c.next {
		fmt.Fprint(w, c.val+" <-> ")
	}
	fmt.Fprintln(w, "null")
}

// PrintReverse - вывод с хвоста
func (l *DList) PrintReverse(w io.Writer) {
	fmt.Fprint(w, "null <-> ")
	for c := l.tail; c != nil; c = c.prev {
		fmt.Fprint(w, c.val+" <-> ")
	}
	fmt.Fprintln(w, "null")
}
