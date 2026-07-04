// typing.cs
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Diagnostics;

class TypingTest
{
    static string Colorize(string text, string color)
    {
        string col = color switch
        {
            "green" => "\x1b[92m",
            "red" => "\x1b[91m",
            "yellow" => "\x1b[93m",
            "blue" => "\x1b[94m",
            "bold" => "\x1b[1m",
            _ => "\x1b[0m"
        };
        return col + text + "\x1b[0m";
    }

    static Dictionary<string, List<string>> TEXTS = new Dictionary<string, List<string>>()
    {
        {"easy", new List<string>{
            "кот и собака играют во дворе",
            "солнце светит ярко сегодня",
            "быстрый лис прыгает через забор"
        }},
        {"medium", new List<string>{
            "программирование это искусство создавать алгоритмы для решения задач",
            "в мире информационных технологий важно постоянно учиться новому",
            "компьютерные науки охватывают множество направлений"
        }},
        {"hard", new List<string>{
            "разработка программного обеспечения требует глубоких знаний и навыков работы в команде",
            "современные технологии изменяют мир и открывают новые возможности для человечества",
            "искусственный интеллект и машинное обучение становятся основой будущих инноваций"
        }}
    };

    private string level;
    private string text;
    private string userInput;
    private DateTime startTime;
    private DateTime endTime;
    private string statsFile;
    private Dictionary<string, double> stats;

    public TypingTest(string level)
    {
        this.level = level;
        var rnd = new Random();
        var list = TEXTS[level];
        text = list[rnd.Next(list.Count)];
        statsFile = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".typing_stats.json");
        LoadStats();
    }

    void LoadStats()
    {
        if (File.Exists(statsFile))
        {
            try
            {
                string json = File.ReadAllText(statsFile);
                stats = JsonSerializer.Deserialize<Dictionary<string, double>>(json);
            }
            catch { stats = new Dictionary<string, double>(); }
        }
        else stats = new Dictionary<string, double>();
    }

    void SaveStats()
    {
        string json = JsonSerializer.Serialize(stats);
        File.WriteAllText(statsFile, json);
    }

    void DisplayText()
    {
        Console.WriteLine(Colorize("\nНапечатайте следующий текст:", "bold"));
        Console.WriteLine(Colorize(text, "blue"));
        Console.WriteLine("\nНажмите Enter после ввода. Время начнётся с первой буквы.");
        Console.Write("Готовы? Нажмите Enter...");
        Console.ReadLine();
        Console.WriteLine("\nНачинайте печатать:");
    }

    void GetUserInput()
    {
        startTime = DateTime.Now;
        userInput = Console.ReadLine();
        endTime = DateTime.Now;
    }

    void CalculateAndDisplay()
    {
        string original = text;
        string typed = userInput ?? "";
        int total = original.Length;
        int typedLen = typed.Length;
        int correct = 0;
        int errors = 0;
        for (int i = 0; i < total; i++)
        {
            if (i < typedLen && original[i] == typed[i])
                correct++;
            else
                errors++;
        }
        if (typedLen < total) errors += total - typedLen;
        double elapsed = (endTime - startTime).TotalSeconds;
        double minutes = elapsed / 60.0;
        double wpm = (correct / 5.0) / minutes;
        double accuracy = (correct / (double)total) * 100.0;

        Console.WriteLine(Colorize("\n📊 Результаты:", "bold"));
        Console.WriteLine($"  Скорость: {wpm:F1} WPM");
        Console.WriteLine($"  Точность: {accuracy:F1}%");
        Console.WriteLine($"  Правильных символов: {correct}/{total}");
        Console.WriteLine($"  Ошибок: {errors}");
        Console.WriteLine($"  Время: {elapsed:F2} сек");

        if (!stats.ContainsKey(level) || wpm > stats[level])
        {
            stats[level] = wpm;
            SaveStats();
            Console.WriteLine(Colorize("🏆 Новый рекорд для этого уровня!", "green"));
        }
        else
        {
            Console.WriteLine($"Лучший результат: {stats[level]:F1} WPM");
        }
    }

    public void Run()
    {
        Console.WriteLine(Colorize("⌨️  Тест скорости печати", "bold"));
        Console.WriteLine($"Уровень: {level}");
        DisplayText();
        GetUserInput();
        CalculateAndDisplay();
    }

    static void Main(string[] args)
    {
        string level = "medium";
        bool showStats = false;
        bool resetStats = false;
        string customText = "";
        for (int i = 0; i < args.Length; i++)
        {
            string arg = args[i];
            if (arg == "easy" || arg == "medium" || arg == "hard")
                level = arg;
            else if (arg == "-t" || arg == "--text")
            {
                if (i + 1 < args.Length)
                    customText = args[++i];
            }
            else if (arg == "-s" || arg == "--stats")
                showStats = true;
            else if (arg == "-r" || arg == "--reset")
                resetStats = true;
            else if (arg == "-h" || arg == "--help")
            {
                Console.WriteLine("Usage: typing [easy|medium|hard] [-t text] [-s] [-r]");
                return;
            }
        }
        if (resetStats)
        {
            string f = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".typing_stats.json");
            if (File.Exists(f)) File.Delete(f);
            Console.WriteLine("Статистика сброшена.");
            return;
        }
        if (showStats)
        {
            string f = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".typing_stats.json");
            if (File.Exists(f))
            {
                try
                {
                    string json = File.ReadAllText(f);
                    var stats = JsonSerializer.Deserialize<Dictionary<string, double>>(json);
                    Console.WriteLine(Colorize("📊 Статистика:", "bold"));
                    foreach (var kv in stats)
                        Console.WriteLine($"  {kv.Key}: {kv.Value:F1} WPM");
                }
                catch { Console.WriteLine("Статистика пуста."); }
            }
            else Console.WriteLine("Статистика пуста.");
            return;
        }
        var test = new TypingTest(level);
        if (!string.IsNullOrEmpty(customText))
            test.text = customText;
        test.Run();
    }
}
