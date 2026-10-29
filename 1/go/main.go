package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

// путь к читаемому txt-файлу рядом с файлом данных
func textPath(file string) string {
	dot := strings.LastIndex(file, ".")
	slash := strings.LastIndexAny(file, "/\\")
	base := file
	if dot != -1 && !(slash != -1 && dot < slash) {
		base = file[:dot]
	}
	out := base + ".txt"
	if out == file {
		out = base + "_view.txt"
	}
	return out
}

// выполнить одну команду и при изменении сохранить файл данных и txt
func runQuery(db *Database, file, query string) bool {
	cmd := strings.Fields(query)
	if len(cmd) == 0 {
		return true
	}
	changed := false
	if !dbExecute(db, cmd, &changed) {
		return false
	}
	if changed && (!dbSave(db, file) || !dbSaveText(db, textPath(file))) {
		fmt.Fprintln(os.Stderr, "ERROR: cannot write file")
		return false
	}
	return true
}

func main() {
	file := "file.data"
	query := ""
	hasQuery := false
	for i := 1; i+1 < len(os.Args); i += 2 {
		switch os.Args[i] {
		case "--file":
			file = os.Args[i+1]
		case "--query":
			query = os.Args[i+1]
			hasQuery = true
		}
	}

	db := &Database{}
	code := 0
	if !dbLoad(db, file) {
		code = 1
	} else if hasQuery {
		if !runQuery(db, file, query) {
			code = 1
		}
	} else {
		sc := bufio.NewScanner(os.Stdin)
		for {
			fmt.Print("> ")
			if !sc.Scan() {
				break
			}
			line := sc.Text()
			if strings.TrimSpace(line) == "exit" {
				break
			}
			runQuery(db, file, line)
		}
	}
	os.Exit(code)
}
