// typing.cpp
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <filesystem>

using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

const string RESET = "\033[0m";
const string GREEN = "\033[92m";
const string RED = "\033[91m";
const string YELLOW = "\033[93m";
const string BLUE = "\033[94m";
const string BOLD = "\033[1m";

string colorize(const string& text, const string& color) {
    return color + text + RESET;
}

string getHomeDir() {
    const char* home = getenv("HOME");
    if (!home) home = getenv("USERPROFILE");
    return string(home);
}

map<string, vector<string>> TEXTS = {
    {"easy", {
        "кот и собака играют во дворе",
        "солнце светит ярко сегодня",
        "быстрый лис прыгает через забор"
    }},
    {"medium", {
        "программирование это искусство создавать алгоритмы для решения задач",
        "в мире информационных технологий важно постоянно учиться новому",
        "компьютерные науки охватывают множество направлений"
    }},
    {"hard", {
        "разработка программного обеспечения требует глубоких знаний и навыков работы в команде",
        "современные технологии изменяют мир и открывают новые возможности для человечества",
        "искусственный интеллект и машинное обучение становятся основой будущих инноваций"
    }}
};

class TypingTest {
public:
    string level;
    string text;
    string userInput;
    high_resolution_clock::time_point startTime, endTime;
    string statsFile;
    map<string, double> stats;

    TypingTest(string lvl) : level(lvl) {
        random_device rd;
        mt19937 gen(rd());
        auto& vec = TEXTS[level];
        uniform_int_distribution<> dis(0, vec.size()-1);
        text = vec[dis(gen)];
        statsFile = getHomeDir() + "/.typing_stats.json";
        loadStats();
    }

    void loadStats() {
        ifstream f(statsFile);
        if (!f) return;
        string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
        // Упрощённый парсинг JSON (без библиотеки)
        size_t pos = content.find("{");
        if (pos == string::npos) return;
        size_t end = content.rfind("}");
        if (end == string::npos) return;
        string json = content.substr(pos, end-pos+1);
        // Парсим пары "key":value
        size_t start = json.find("\"");
        while (start != string::npos) {
            size_t keyEnd = json.find("\"", start+1);
            if (keyEnd == string::npos) break;
            string key = json.substr(start+1, keyEnd-start-1);
            size_t valStart = json.find(":", keyEnd);
            if (valStart == string::npos) break;
            size_t valEnd = json.find(",", valStart+1);
            if (valEnd == string::npos) valEnd = json.find("}", valStart+1);
            if (valEnd == string::npos) break;
            string valStr = json.substr(valStart+1, valEnd-valStart-1);
            // Удаляем пробелы и кавычки
            valStr.erase(remove_if(valStr.begin(), valStr.end(), ::isspace), valStr.end());
            if (valStr.front() == '"') valStr.erase(0,1);
            if (valStr.back() == '"') valStr.pop_back();
            try {
                double val = stod(valStr);
                stats[key] = val;
            } catch (...) {}
            start = json.find("\"", valEnd);
        }
    }

    void saveStats() {
        ofstream f(statsFile);
        if (f) {
            f << "{";
            bool first = true;
            for (auto& kv : stats) {
                if (!first) f << ",";
                first = false;
                f << "\"" << kv.first << "\":" << kv.second;
            }
            f << "}";
        }
    }

    void displayText() {
        cout << colorize("\nНапечатайте следующий текст:", BOLD) << endl;
        cout << colorize(text, BLUE) << endl;
        cout << "\nНажмите Enter после ввода. Время начнётся с первой буквы." << endl;
        cout << "Готовы? Нажмите Enter...";
        cin.ignore();
        cout << "\nНачинайте печатать:" << endl;
    }

    void getUserInput() {
        startTime = high_resolution_clock::now();
        getline(cin, userInput);
        endTime = high_resolution_clock::now();
    }

    void calculateAndDisplay() {
        string original = text;
        string typed = userInput;
        int total = original.size();
        int typedLen = typed.size();
        int correct = 0;
        int errors = 0;
        for (int i=0; i<total; ++i) {
            if (i < typedLen && original[i] == typed[i]) {
                correct++;
            } else {
                errors++;
            }
        }
        if (typedLen < total) errors += total - typedLen;
        auto duration = duration_cast<milliseconds>(endTime - startTime).count();
        double seconds = duration / 1000.0;
        double minutes = seconds / 60.0;
        double wpm = (correct / 5.0) / minutes;
        double accuracy = (correct / (double)total) * 100.0;

        cout << colorize("\n📊 Результаты:", BOLD) << endl;
        cout << "  Скорость: " << wpm << " WPM" << endl;
        cout << "  Точность: " << accuracy << "%" << endl;
        cout << "  Правильных символов: " << correct << "/" << total << endl;
        cout << "  Ошибок: " << errors << endl;
        cout << "  Время: " << seconds << " сек" << endl;

        // Сохранение рекорда
        if (stats.find(level) == stats.end() || wpm > stats[level]) {
            stats[level] = wpm;
            saveStats();
            cout << colorize("🏆 Новый рекорд для этого уровня!", GREEN) << endl;
        } else {
            cout << "Лучший результат: " << stats[level] << " WPM" << endl;
        }
    }

    void run() {
        cout << colorize("⌨️  Тест скорости печати", BOLD) << endl;
        cout << "Уровень: " << level << endl;
        displayText();
        getUserInput();
        calculateAndDisplay();
    }
};

int main(int argc, char* argv[]) {
    string level = "medium";
    bool showStats = false, resetStats = false;
    string customText = "";
    for (int i=1; i<argc; ++i) {
        string arg = argv[i];
        if (arg == "easy" || arg == "medium" || arg == "hard") {
            level = arg;
        } else if (arg == "-t" || arg == "--text") {
            if (i+1 < argc) {
                customText = argv[++i];
            }
        } else if (arg == "-s" || arg == "--stats") {
            showStats = true;
        } else if (arg == "-r" || arg == "--reset") {
            resetStats = true;
        } else if (arg == "-h" || arg == "--help") {
            cout << "Usage: typing [easy|medium|hard] [-t text] [-s] [-r]" << endl;
            return 0;
        }
    }
    if (resetStats) {
        string f = getHomeDir() + "/.typing_stats.json";
        if (fs::exists(f)) fs::remove(f);
        cout << "Статистика сброшена." << endl;
        return 0;
    }
    if (showStats) {
        string f = getHomeDir() + "/.typing_stats.json";
        ifstream file(f);
        if (file) {
            string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            // Простой вывод (для демонстрации)
            cout << colorize("📊 Статистика:", BOLD) << endl;
            // Парсим и выводим
            size_t pos = content.find("{");
            if (pos != string::npos) {
                size_t end = content.rfind("}");
                if (end != string::npos) {
                    string json = content.substr(pos+1, end-pos-1);
                    size_t start = 0;
                    while ((start = json.find("\"", start)) != string::npos) {
                        size_t keyEnd = json.find("\"", start+1);
                        if (keyEnd == string::npos) break;
                        string key = json.substr(start+1, keyEnd-start-1);
                        size_t valStart = json.find(":", keyEnd);
                        if (valStart == string::npos) break;
                        size_t valEnd = json.find(",", valStart+1);
                        if (valEnd == string::npos) valEnd = json.size();
                        string valStr = json.substr(valStart+1, valEnd-valStart-1);
                        valStr.erase(remove_if(valStr.begin(), valStr.end(), ::isspace), valStr.end());
                        if (valStr.front() == '"') valStr.erase(0,1);
                        if (valStr.back() == '"') valStr.pop_back();
                        try {
                            double val = stod(valStr);
                            cout << "  " << key << ": " << val << " WPM" << endl;
                        } catch (...) {}
                        start = valEnd;
                    }
                }
            }
        } else {
            cout << "Статистика пуста." << endl;
        }
        return 0;
    }
    if (!customText.empty()) {
        TypingTest test(level);
        test.text = customText;
        test.run();
    } else {
        TypingTest test(level);
        test.run();
    }
    return 0;
}
