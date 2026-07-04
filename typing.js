// typing.js
#!/usr/bin/env node
'use strict';

const fs = require('fs');
const path = require('path');
const os = require('os');
const readline = require('readline');

const COLORS = {
    reset: '\x1b[0m',
    green: '\x1b[92m',
    red: '\x1b[91m',
    yellow: '\x1b[93m',
    blue: '\x1b[94m',
    bold: '\x1b[1m'
};

function colorize(text, color) {
    return COLORS[color] + text + COLORS.reset;
}

const TEXTS = {
    easy: [
        'кот и собака играют во дворе',
        'солнце светит ярко сегодня',
        'быстрый лис прыгает через забор'
    ],
    medium: [
        'программирование это искусство создавать алгоритмы для решения задач',
        'в мире информационных технологий важно постоянно учиться новому',
        'компьютерные науки охватывают множество направлений'
    ],
    hard: [
        'разработка программного обеспечения требует глубоких знаний и навыков работы в команде',
        'современные технологии изменяют мир и открывают новые возможности для человечества',
        'искусственный интеллект и машинное обучение становятся основой будущих инноваций'
    ]
};

class TypingTest {
    constructor(level = 'medium') {
        this.level = level;
        const list = TEXTS[level];
        this.text = list[Math.floor(Math.random() * list.length)];
        this.userInput = '';
        this.startTime = null;
        this.endTime = null;
        this.statsFile = path.join(os.homedir(), '.typing_stats.json');
        this.loadStats();
    }

    loadStats() {
        try {
            this.stats = JSON.parse(fs.readFileSync(this.statsFile, 'utf8'));
        } catch {
            this.stats = {};
        }
    }

    saveStats() {
        fs.writeFileSync(this.statsFile, JSON.stringify(this.stats, null, 2));
    }

    displayText() {
        console.log(colorize('\nНапечатайте следующий текст:', 'bold'));
        console.log(colorize(this.text, 'blue'));
        console.log('\nНажмите Enter после ввода. Время начнётся с первой буквы.');
        return new Promise(resolve => {
            const rl = readline.createInterface({
                input: process.stdin,
                output: process.stdout
            });
            rl.question('Готовы? Нажмите Enter...', () => {
                rl.close();
                resolve();
            });
        });
    }

    async getUserInput() {
        console.log('\nНачинайте печатать:');
        const rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout
        });
        this.startTime = Date.now();
        const input = await new Promise(resolve => {
            rl.question('', answer => {
                resolve(answer);
                rl.close();
            });
        });
        this.userInput = input;
        this.endTime = Date.now();
    }

    calculateAndDisplay() {
        const original = this.text;
        const typed = this.userInput;
        const total = original.length;
        const typedLen = typed.length;
        let correct = 0;
        let errors = 0;
        for (let i = 0; i < total; i++) {
            if (i < typedLen && original[i] === typed[i]) {
                correct++;
            } else {
                errors++;
            }
        }
        if (typedLen < total) errors += total - typedLen;
        const elapsed = (this.endTime - this.startTime) / 1000;
        const minutes = elapsed / 60;
        const wpm = (correct / 5) / minutes;
        const accuracy = (correct / total) * 100;

        console.log(colorize('\n📊 Результаты:', 'bold'));
        console.log(`  Скорость: ${wpm.toFixed(1)} WPM`);
        console.log(`  Точность: ${accuracy.toFixed(1)}%`);
        console.log(`  Правильных символов: ${correct}/${total}`);
        console.log(`  Ошибок: ${errors}`);
        console.log(`  Время: ${elapsed.toFixed(2)} сек`);

        if (!this.stats[this.level] || wpm > this.stats[this.level]) {
            this.stats[this.level] = wpm;
            this.saveStats();
            console.log(colorize('🏆 Новый рекорд для этого уровня!', 'green'));
        } else {
            console.log(`Лучший результат: ${this.stats[this.level].toFixed(1)} WPM`);
        }
    }

    async run() {
        console.log(colorize('⌨️  Тест скорости печати', 'bold'));
        console.log(`Уровень: ${this.level}`);
        await this.displayText();
        await this.getUserInput();
        this.calculateAndDisplay();
    }
}

async function main() {
    let level = 'medium';
    let showStats = false;
    let resetStats = false;
    let customText = '';
    const args = process.argv.slice(2);
    for (let i = 0; i < args.length; i++) {
        const arg = args[i];
        if (arg === 'easy' || arg === 'medium' || arg === 'hard') {
            level = arg;
        } else if (arg === '-t' || arg === '--text') {
            if (i+1 < args.length) {
                customText = args[++i];
            }
        } else if (arg === '-s' || arg === '--stats') {
            showStats = true;
        } else if (arg === '-r' || arg === '--reset') {
            resetStats = true;
        } else if (arg === '-h' || arg === '--help') {
            console.log('Usage: node typing.js [easy|medium|hard] [-t text] [-s] [-r]');
            return;
        }
    }
    if (resetStats) {
        const f = path.join(os.homedir(), '.typing_stats.json');
        if (fs.existsSync(f)) fs.unlinkSync(f);
        console.log('Статистика сброшена.');
        return;
    }
    if (showStats) {
        const f = path.join(os.homedir(), '.typing_stats.json');
        try {
            const stats = JSON.parse(fs.readFileSync(f, 'utf8'));
            console.log(colorize('📊 Статистика:', 'bold'));
            for (const [lvl, wpm] of Object.entries(stats)) {
                console.log(`  ${lvl}: ${wpm.toFixed(1)} WPM`);
            }
        } catch {
            console.log('Статистика пуста.');
        }
        return;
    }
    const test = new TypingTest(level);
    if (customText) {
        test.text = customText;
    }
    await test.run();
}

main().catch(console.error);
