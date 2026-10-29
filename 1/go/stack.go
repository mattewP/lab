package main

import (
	"fmt"
	"io"
)

// Stack - стек на связном списке, вершина top
type Stack struct {
	top *SNode
	n   int
}

// Push - положить на вершину
func (s *Stack) Push(v string) {
	s.top = &SNode{val: v, next: s.top}
	s.n++
}

// Pop - взять и удалить вершину
func (s *Stack) Pop() (string, bool) {
	if s.top == nil {
		return "", false
	}
	v := s.top.val
	s.top = s.top.next
	s.n--
	return v, true
}

// Peek - прочитать вершину без удаления
func (s *Stack) Peek() (string, bool) {
	if s.top == nil {
		return "", false
	}
	return s.top.val, true
}

// Print - вывод от вершины вниз
func (s *Stack) Print(w io.Writer) {
	fmt.Fprint(w, "top -> ")
	for c := s.top; c != nil; c = c.next {
		fmt.Fprint(w, c.val+" ")
	}
	fmt.Fprintln(w)
}
