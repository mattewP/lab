package main

import (
	"fmt"
	"io"
)

// SNode - узел односвязного списка (используется также стеком и очередью)
type SNode struct {
	val  string
	next *SNode
}

// SList - односвязный список с указателями на голову и хвост
type SList struct {
	head *SNode
	tail *SNode
	n    int
}

// PushHead - добавить в голову
func (l *SList) PushHead(v string) {
	nd := &SNode{val: v, next: l.head}
	l.head = nd
	if l.tail == nil {
		l.tail = nd
	}
	l.n++
}

// PushTail - добавить в хвост
func (l *SList) PushTail(v string) {
	nd := &SNode{val: v}
	if l.tail != nil {
		l.tail.next = nd
	} else {
		l.head = nd
	}
	l.tail = nd
	l.n++
}

// PushAfter - добавить после элемента со значением target
func (l *SList) PushAfter(target, v string) bool {
	for c := l.head; c != nil; c = c.next {
		if c.val == target {
			c.next = &SNode{val: v, next: c.next}
			if c == l.tail {
				l.tail = c.next
			}
			l.n++
			return true
		}
	}
	return false
}

// PushBefore - добавить перед элементом со значением target
func (l *SList) PushBefore(target, v string) bool {
	if l.head == nil {
		return false
	}
	if l.head.val == target {
		l.PushHead(v)
		return true
	}
	for c := l.head; c.next != nil; c = c.next {
		if c.next.val == target {
			c.next = &SNode{val: v, next: c.next}
			l.n++
			return true
		}
	}
	return false
}

// DelHead - удалить голову
func (l *SList) DelHead() bool {
	if l.head == nil {
		return false
	}
	l.head = l.head.next
	if l.head == nil {
		l.tail = nil
	}
	l.n--
	return true
}

// DelTail - удалить хвост
func (l *SList) DelTail() bool {
	if l.head == nil {
		return false
	}
	if l.head == l.tail {
		return l.DelHead()
	}
	c := l.head
	for c.next != l.tail {
		c = c.next
	}
	l.tail = c
	c.next = nil
	l.n--
	return true
}

// DelAfter - удалить элемент после target
func (l *SList) DelAfter(target string) bool {
	for c := l.head; c != nil; c = c.next {
		if c.val == target {
			t := c.next
			if t == nil {
				return false
			}
			c.next = t.next
			if t == l.tail {
				l.tail = c
			}
			l.n--
			return true
		}
	}
	return false
}

// DelBefore - удалить элемент перед target
func (l *SList) DelBefore(target string) bool {
	if l.head == nil || l.head.val == target {
		return false
	}
	if l.head.next != nil && l.head.next.val == target {
		return l.DelHead()
	}
	for c := l.head; c.next != nil && c.next.next != nil; c = c.next {
		if c.next.next.val == target {
			c.next = c.next.next
			l.n--
			return true
		}
	}
	return false
}

// DelValue - удалить первое вхождение значения
func (l *SList) DelValue(v string) bool {
	if l.head == nil {
		return false
	}
	if l.head.val == v {
		return l.DelHead()
	}
	for c := l.head; c.next != nil; c = c.next {
		if c.next.val == v {
			t := c.next
			c.next = t.next
			if t == l.tail {
				l.tail = c
			}
			l.n--
			return true
		}
	}
	return false
}

// Find - индекс первого вхождения или -1
func (l *SList) Find(v string) int {
	i := 0
	for c := l.head; c != nil; c = c.next {
		if c.val == v {
			return i
		}
		i++
	}
	return -1
}

// Get - прочитать по индексу
func (l *SList) Get(idx int) (string, bool) {
	if idx < 0 || idx >= l.n {
		return "", false
	}
	c := l.head
	for ; idx > 0; idx-- {
		c = c.next
	}
	return c.val, true
}

// Front - значение головы
func (l *SList) Front() (string, bool) {
	if l.head == nil {
		return "", false
	}
	return l.head.val, true
}

// Back - значение хвоста
func (l *SList) Back() (string, bool) {
	if l.tail == nil {
		return "", false
	}
	return l.tail.val, true
}

// Print - вывод вида a -> b -> null
func (l *SList) Print(w io.Writer) {
	for c := l.head; c != nil; c = c.next {
		fmt.Fprint(w, c.val+" -> ")
	}
	fmt.Fprintln(w, "null")
}
