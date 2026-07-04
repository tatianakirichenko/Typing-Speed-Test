# typing.py
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import random
import json
import time
from pathlib import Path

# ANSI-цвета
COLORS = {
    'reset': '\033[0m',
    'green': '\033[92m',
    'red': '\033[91m',
    'yellow': '\033[93m',
    'blue': '\033[94m',
    'bold': '\033[1m'
}

def colorize(text, color):
    return f"{COLORS.get(color, '')}{text}{COLORS['reset']}"

# Встроенные тексты по уровням
TEXTS = {
    'easy': [
        "кот и собака играют во дворе",
        "солнце светит ярко сегодня",
        "быстрый лис прыгает через забор"
    ],
    'medium': [
        "программирование это искусство создавать алгоритмы для решения задач",
        "в мире информационных технологий важно постоянно учиться новому",
        "компьютерные науки охватывают множество направлений"
    ],
    'hard': [
        "разработка программного обеспечения требует глубоких знаний и навыков работы в команде",
        "современные технологии изменяют мир и открывают новые возможности для человечества",
        "искусственный интеллект и машинное обучение становятся основой будущих инноваций"
    ]
}

class TypingTest:
    def __init__(self, level='medium'):
        self.level = level
        self.text = random.choice(TEXTS[level])
        self.start_time = None
        self.end_time = None
        self.user_input = ""
        self.stats_file = Path.home() / '.typing_stats.json'
        self.load_stats()

    def load_stats(self):
        if self.stats_file.exists():
            with open(self.stats_file, 'r') as f:
                self.stats = json.load(f)
        else:
            self.stats = {}

    def save_stats(self):
        with open(self.stats_file, 'w') as f:
            json.dump(self.stats, f, indent=2)

    def display_text(self):
        print(colorize("\nНапечатайте следующий текст:", 'bold'))
        print(colorize(self.text, 'blue'))
        print("\nНажмите Enter после ввода. Время начнётся с первой буквы.")
        input("Готовы? Нажмите Enter...")
        print("\nНачинайте печатать:")

    def get_user_input(self):
        self.start_time = time.time()
        self.user_input = input()
        self.end_time = time.time()

    def calculate_results(self):
        original = self.text
        typed = self.user_input
        total_chars = len(original)
        typed_chars = len(typed)
        correct = 0
        errors = 0
        for i, ch in enumerate(original):
            if i < typed_chars and ch == typed[i]:
                correct += 1
            else:
                errors += 1
        # Если текст короче оригинала, считаем остальные как ошибки
        if typed_chars < total_chars:
            errors += total_chars - typed_chars
        elapsed = self.end_time - self.start_time
        minutes = elapsed / 60.0
        # Количество слов: делим на 5 (стандарт)
        word_count = correct / 5.0
        wpm = word_count / minutes if minutes > 0 else 0
        accuracy = (correct / total_chars) * 100 if total_chars > 0 else 0
        return {
            'wpm': wpm,
            'accuracy': accuracy,
            'correct': correct,
            'errors': errors,
            'total': total_chars,
            'time': elapsed
        }

    def display_results(self, results):
        print(colorize("\n📊 Результаты:", 'bold'))
        print(f"  Скорость: {results['wpm']:.1f} WPM")
        print(f"  Точность: {results['accuracy']:.1f}%")
        print(f"  Правильных символов: {results['correct']}/{results['total']}")
        print(f"  Ошибок: {results['errors']}")
        print(f"  Время: {results['time']:.2f} сек")

        # Сохранение лучшего результата
        if self.level not in self.stats or results['wpm'] > self.stats[self.level]:
            self.stats[self.level] = results['wpm']
            self.save_stats()
            print(colorize("🏆 Новый рекорд для этого уровня!", 'green'))
        else:
            print(f"Лучший результат: {self.stats.get(self.level, 0):.1f} WPM")

    def run(self):
        print(colorize("⌨️  Тест скорости печати", 'bold'))
        print(f"Уровень: {self.level}")
        self.display_text()
        self.get_user_input()
        results = self.calculate_results()
        self.display_results(results)

def main():
    level = 'medium'
    show_stats = False
    reset_stats = False
    custom_text = None
    args = sys.argv[1:]
    for i, arg in enumerate(args):
        if arg in ['easy', 'medium', 'hard']:
            level = arg
        elif arg == '-t' or arg == '--text':
            if i+1 < len(args):
                custom_text = args[i+1]
        elif arg == '-s' or arg == '--stats':
            show_stats = True
        elif arg == '-r' or arg == '--reset':
            reset_stats = True
        elif arg == '-h' or arg == '--help':
            print("Usage: typing.py [easy|medium|hard] [-t text] [-s] [-r]")
            return
    if reset_stats:
        stats_file = Path.home() / '.typing_stats.json'
        if stats_file.exists():
            stats_file.unlink()
        print("Статистика сброшена.")
        return
    if show_stats:
        stats_file = Path.home() / '.typing_stats.json'
        if stats_file.exists():
            with open(stats_file, 'r') as f:
                stats = json.load(f)
                print(colorize("📊 Статистика:", 'bold'))
                for lvl, wpm in stats.items():
                    print(f"  {lvl}: {wpm:.1f} WPM")
        else:
            print("Статистика пуста.")
        return
    if custom_text:
        # Используем пользовательский текст
        test = TypingTest(level)
        test.text = custom_text
        test.run()
    else:
        test = TypingTest(level)
        test.run()

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print(colorize("\nТест прерван.", 'yellow'))
        sys.exit(0)
