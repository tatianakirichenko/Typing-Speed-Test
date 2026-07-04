// typing.go
package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"math/rand"
	"os"
	"path/filepath"
	"strings"
	"time"
)

const (
	reset  = "\033[0m"
	green  = "\033[92m"
	red    = "\033[91m"
	yellow = "\033[93m"
	blue   = "\033[94m"
	bold   = "\033[1m"
)

func colorize(text, color string) string {
	return color + text + reset
}

var texts = map[string][]string{
	"easy": {
		"кот и собака играют во дворе",
		"солнце светит ярко сегодня",
		"быстрый лис прыгает через забор",
	},
	"medium": {
		"программирование это искусство создавать алгоритмы для решения задач",
		"в мире информационных технологий важно постоянно учиться новому",
		"компьютерные науки охватывают множество направлений",
	},
	"hard": {
		"разработка программного обеспечения требует глубоких знаний и навыков работы в команде",
		"современные технологии изменяют мир и открывают новые возможности для человечества",
		"искусственный интеллект и машинное обучение становятся основой будущих инноваций",
	},
}

type Stats map[string]float64

type TypingTest struct {
	level      string
	text       string
	userInput  string
	startTime  time.Time
	endTime    time.Time
	statsFile  string
	stats      Stats
}

func NewTypingTest(level string) *TypingTest {
	t := &TypingTest{level: level}
	t.statsFile = filepath.Join(os.Getenv("HOME"), ".typing_stats.json")
	t.loadStats()
	rand.Seed(time.Now().UnixNano())
	list := texts[level]
	t.text = list[rand.Intn(len(list))]
	return t
}

func (t *TypingTest) loadStats() {
	data, err := os.ReadFile(t.statsFile)
	if err != nil {
		t.stats = make(Stats)
		return
	}
	json.Unmarshal(data, &t.stats)
}

func (t *TypingTest) saveStats() {
	data, _ := json.MarshalIndent(t.stats, "", "  ")
	os.WriteFile(t.statsFile, data, 0644)
}

func (t *TypingTest) displayText() {
	fmt.Println(colorize("\nНапечатайте следующий текст:", bold))
	fmt.Println(colorize(t.text, blue))
	fmt.Println("\nНажмите Enter после ввода. Время начнётся с первой буквы.")
	fmt.Print("Готовы? Нажмите Enter...")
	bufio.NewReader(os.Stdin).ReadBytes('\n')
	fmt.Println("\nНачинайте печатать:")
}

func (t *TypingTest) getUserInput() {
	t.startTime = time.Now()
	reader := bufio.NewReader(os.Stdin)
	input, _ := reader.ReadString('\n')
	t.userInput = strings.TrimRight(input, "\n")
	t.endTime = time.Now()
}

func (t *TypingTest) calculateAndDisplay() {
	original := t.text
	typed := t.userInput
	total := len(original)
	typedLen := len(typed)
	correct := 0
	errors := 0
	for i := 0; i < total; i++ {
		if i < typedLen && original[i] == typed[i] {
			correct++
		} else {
			errors++
		}
	}
	if typedLen < total {
		errors += total - typedLen
	}
	elapsed := t.endTime.Sub(t.startTime).Seconds()
	minutes := elapsed / 60.0
	wpm := (float64(correct) / 5.0) / minutes
	accuracy := (float64(correct) / float64(total)) * 100.0

	fmt.Println(colorize("\n📊 Результаты:", bold))
	fmt.Printf("  Скорость: %.1f WPM\n", wpm)
	fmt.Printf("  Точность: %.1f%%\n", accuracy)
	fmt.Printf("  Правильных символов: %d/%d\n", correct, total)
	fmt.Printf("  Ошибок: %d\n", errors)
	fmt.Printf("  Время: %.2f сек\n", elapsed)

	if val, ok := t.stats[t.level]; !ok || wpm > val {
		t.stats[t.level] = wpm
		t.saveStats()
		fmt.Println(colorize("🏆 Новый рекорд для этого уровня!", green))
	} else {
		fmt.Printf("Лучший результат: %.1f WPM\n", t.stats[t.level])
	}
}

func (t *TypingTest) run() {
	fmt.Println(colorize("⌨️  Тест скорости печати", bold))
	fmt.Printf("Уровень: %s\n", t.level)
	t.displayText()
	t.getUserInput()
	t.calculateAndDisplay()
}

func main() {
	level := "medium"
	showStats := false
	resetStats := false
	customText := ""
	args := os.Args[1:]
	for i := 0; i < len(args); i++ {
		arg := args[i]
		switch arg {
		case "easy", "medium", "hard":
			level = arg
		case "-t", "--text":
			if i+1 < len(args) {
				customText = args[i+1]
				i++
			}
		case "-s", "--stats":
			showStats = true
		case "-r", "--reset":
			resetStats = true
		case "-h", "--help":
			fmt.Println("Usage: typing [easy|medium|hard] [-t text] [-s] [-r]")
			return
		}
	}
	if resetStats {
		f := filepath.Join(os.Getenv("HOME"), ".typing_stats.json")
		os.Remove(f)
		fmt.Println("Статистика сброшена.")
		return
	}
	if showStats {
		f := filepath.Join(os.Getenv("HOME"), ".typing_stats.json")
		data, err := os.ReadFile(f)
		if err != nil {
			fmt.Println("Статистика пуста.")
			return
		}
		var stats Stats
		json.Unmarshal(data, &stats)
		fmt.Println(colorize("📊 Статистика:", bold))
		for lvl, wpm := range stats {
			fmt.Printf("  %s: %.1f WPM\n", lvl, wpm)
		}
		return
	}
	if customText != "" {
		test := NewTypingTest(level)
		test.text = customText
		test.run()
	} else {
		test := NewTypingTest(level)
		test.run()
	}
}
