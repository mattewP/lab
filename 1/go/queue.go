package main

import (
	"fmt"
	"io"
)

// Queue - очередь на связном списке: добавление в хвост, удаление из головы
type Queue struct {
	head *SNode
	tail *SNode
	n    int
}

// Push - добавить в конец очереди
func (q *Queue) Push(v string) {
	nd := &SNode{val: v}
	if q.tail != nil {
		q.tail.next = nd
	} else {
		q.head = nd
	}
	q.tail = nd
	q.n++
}

// Pop - взять и удалить первый элемент
func (q *Queue) Pop() (string, bool) {
	if q.head == nil {
		return "", false
	}
	v := q.head.val
	q.head = q.head.next
	if q.head == nil {
		q.tail = nil
	}
	q.n--
	return v, true
}

// Peek - прочитать первый элемент без удаления
func (q *Queue) Peek() (string, bool) {
	if q.head == nil {
		return "", false
	}
	return q.head.val, true
}

// Print - вывод от начала к концу
func (q *Queue) Print(w io.Writer) {
	fmt.Fprint(w, "front -> ")
	for c := q.head; c != nil; c = c.next {
		fmt.Fprint(w, c.val+" ")
	}
	fmt.Fprintln(w, "<- back")
}
