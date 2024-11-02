package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func main() {
	args := os.Args[1:]
	if len(args) < 1 {
		panic("incorrect args")
	}
	if args[0] == "a" {
		// pcidt - pipe communication id table
		file, err := os.OpenFile("/tmp/pcidt.yaml", os.O_APPEND|os.O_WRONLY|os.O_CREATE, 0644)
		if err != nil {
			panic(err)
		}
		file.WriteString(fmt.Sprintf("%s: %s\n", args[1], args[2]))
		defer file.Close()
	} else if args[0] == "g" {
		os.WriteFile("/tmp/pco.txt", []byte(""), 0644)
		if len(args) < 2 {
			panic("missing key for 'g' command")
		}
		key := args[1]
		value := ""
		var lines []string

		file, err := os.Open("/tmp/pcidt.yaml")
		if err != nil {
			panic(err)
		}
		defer file.Close()

		scanner := bufio.NewScanner(file)
		for scanner.Scan() {
			line := scanner.Text()
			if strings.HasPrefix(line, key+":") {
				parts := strings.SplitN(line, ": ", 2)
				if len(parts) == 2 {
					value = parts[1]
				}
			} else {
				lines = append(lines, line)
			}
		}

		if err := scanner.Err(); err != nil {
			panic(err)
		}

		if value != "" {
			outputFile, err := os.OpenFile("/tmp/pco.txt", os.O_APPEND|os.O_WRONLY|os.O_CREATE, 0644)
			if err != nil {
				panic(err)
			}
			defer outputFile.Close()

			_, err = outputFile.WriteString(value + "\n")
			if err != nil {
				panic(err)
			}

			file, err := os.OpenFile("/tmp/pcidt.yaml", os.O_WRONLY|os.O_TRUNC, 0644)
			if err != nil {
				panic(err)
			}
			defer file.Close()

			for _, line := range lines {
				_, err = file.WriteString(line + "\n")
				if err != nil {
					panic(err)
				}
			}
		}
	}
}
