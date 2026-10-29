package main

import (
	"fmt"
	"io"
)

// Array - динамический массив: ёмкость равна len(data), число элементов хранится в n
type Array struct {
	data []string
	n    int
}

// удвоение ёмкости
func (a *Array) grow() {
	ncap := 4
	if len(a.data) > 0 {
		ncap = len(a.data) * 2
	}
	nd := make([]string, ncap)
	copy(nd, a.data[:a.n])
	a.data = nd
}

// PushBack - добавить в конец
func (a *Array) PushBack(v string) {
	if a.n == len(a.data) {
		a.grow()
	}
	a.data[a.n] = v
	a.n++
}

// Insert - вставка по индексу
func (a *Array) Insert(idx int, v string) bool {
	if idx < 0 || idx > a.n {
		return false
	}
	if a.n == len(a.data) {
		a.grow()
	}
	for i := a.n; i > idx; i-- {
		a.data[i] = a.data[i-1]
	}
	a.data[idx] = v
	a.n++
	return true
}

// Get - получить по индексу
func (a *Array) Get(idx int) (string, bool) {
	if idx < 0 || idx >= a.n {
		return "", false
	}
	return a.data[idx], true
}

// Del - удалить по индексу
func (a *Array) Del(idx int) bool {
	if idx < 0 || idx >= a.n {
		return false
	}
	for i := idx; i+1 < a.n; i++ {
		a.data[i] = a.data[i+1]
	}
	a.data[a.n-1] = ""
	a.n--
	return true
}

// Set - заменить по индексу
func (a *Array) Set(idx int, v string) bool {
	if idx < 0 || idx >= a.n {
		return false
	}
	a.data[idx] = v
	return true
}

// Print - вывод в виде [a, b, c]
func (a *Array) Print(w io.Writer) {
	fmt.Fprint(w, "[")
	for i := 0; i < a.n; i++ {
		if i > 0 {
			fmt.Fprint(w, ", ")
		}
		fmt.Fprint(w, a.data[i])
	}
	fmt.Fprintln(w, "]")
}
