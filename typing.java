// typing.java
import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.util.stream.*;

public class typing {
    private static final String RESET = "\u001B[0m";
    private static final String GREEN = "\u001B[92m";
    private static final String RED = "\u001B[91m";
    private static final String YELLOW = "\u001B[93m";
    private static final String BLUE = "\u001B[94m";
    private static final String BOLD = "\u001B[1m";

    private static String colorize(String text, String color) {
        return color + text + RESET;
    }

    private static final Map<String, List<String>> TEXTS = new HashMap<>();
    static {
        TEXTS.put("easy", Arrays.asList(
            "кот и собака играют во дворе",
            "солнце светит ярко сегодня",
            "быстрый лис прыгает через забор"
        ));
        TEXTS.put("medium", Arrays.asList(
            "программирование это искусство создавать алгоритмы для решения задач",
            "в мире информационных технологий важно постоянно учиться новому",
            "компьютерные науки охватывают множество направлений"
        ));
        TEXTS.put("hard", Arrays.asList(
            "разработка программного обеспечения требует глубоких знаний и навыков работы в команде",
            "современные технологии изменяют мир и открывают новые возможности для человечества",
            "искусственный интеллект и машинное обучение становятся основой будущих инноваций"
        ));
    }

    private String level;
    private String text;
    private String userInput;
    private long startTime;
    private long endTime;
    private String statsFile;
    private Map<String, Double> stats;
    private Scanner scanner;

    public typing(String level) {
        this.level = level;
        List<String> list = TEXTS.get(level);
        Random rnd = new Random();
        text = list.get(rnd.nextInt(list.size()));
        statsFile = System.getProperty("user.home") + "/.typing_stats.json";
        loadStats();
        scanner = new Scanner(System.in);
    }

    private void loadStats() {
        stats = new HashMap<>();
        try {
            String json = new String(Files.readAllBytes(Paths.get(statsFile)));
            // Упрощённый парсинг JSON (без библиотеки)
            json = json.trim();
            if (json.startsWith("{") && json.endsWith("}")) {
                json = json.substring(1, json.length()-1);
                String[] pairs = json.split(",");
                for (String pair : pairs) {
                    String[] kv = pair.split(":");
                    if (kv.length == 2) {
                        String key = kv[0].trim().replaceAll("\"", "");
                        double val = Double.parseDouble(kv[1].trim());
                        stats.put(key, val);
                    }
                }
            }
        } catch (Exception e) {
            // file not exists or invalid
        }
    }

    private void saveStats() {
        try {
            StringBuilder sb = new StringBuilder("{");
            boolean first = true;
            for (Map.Entry<String, Double> e : stats.entrySet()) {
                if (!first) sb.append(",");
                first = false;
                sb.append("\"").append(e.getKey()).append("\":").append(e.getValue());
            }
            sb.append("}");
            Files.write(Paths.get(statsFile), sb.toString().getBytes());
        } catch (IOException e) {}
    }

    private void displayText() {
        System.out.println(colorize("\nНапечатайте следующий текст:", BOLD));
        System.out.println(colorize(text, BLUE));
        System.out.println("\nНажмите Enter после ввода. Время начнётся с первой буквы.");
        System.out.print("Готовы? Нажмите Enter...");
        scanner.nextLine();
        System.out.println("\nНачинайте печатать:");
    }

    private void getUserInput() {
        startTime = System.currentTimeMillis();
        userInput = scanner.nextLine();
        endTime = System.currentTimeMillis();
    }

    private void calculateAndDisplay() {
        String original = text;
        String typed = userInput != null ? userInput : "";
        int total = original.length();
        int typedLen = typed.length();
        int correct = 0;
        int errors = 0;
        for (int i = 0; i < total; i++) {
            if (i < typedLen && original.charAt(i) == typed.charAt(i)) {
                correct++;
            } else {
                errors++;
            }
        }
        if (typedLen < total) errors += total - typedLen;
        double elapsed = (endTime - startTime) / 1000.0;
        double minutes = elapsed / 60.0;
        double wpm = (correct / 5.0) / minutes;
        double accuracy = (correct / (double) total) * 100.0;

        System.out.println(colorize("\n📊 Результаты:", BOLD));
        System.out.printf("  Скорость: %.1f WPM\n", wpm);
        System.out.printf("  Точность: %.1f%%\n", accuracy);
        System.out.printf("  Правильных символов: %d/%d\n", correct, total);
        System.out.printf("  Ошибок: %d\n", errors);
        System.out.printf("  Время: %.2f сек\n", elapsed);

        if (!stats.containsKey(level) || wpm > stats.get(level)) {
            stats.put(level, wpm);
            saveStats();
            System.out.println(colorize("🏆 Новый рекорд для этого уровня!", GREEN));
        } else {
            System.out.printf("Лучший результат: %.1f WPM\n", stats.get(level));
        }
    }

    public void run() {
        System.out.println(colorize("⌨️  Тест скорости печати", BOLD));
        System.out.println("Уровень: " + level);
        displayText();
        getUserInput();
        calculateAndDisplay();
        scanner.close();
    }

    public static void main(String[] args) {
        String level = "medium";
        boolean showStats = false;
        boolean resetStats = false;
        String customText = "";
        for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if (arg.equals("easy") || arg.equals("medium") || arg.equals("hard")) {
                level = arg;
            } else if (arg.equals("-t") || arg.equals("--text")) {
                if (i+1 < args.length) {
                    customText = args[++i];
                }
            } else if (arg.equals("-s") || arg.equals("--stats")) {
                showStats = true;
            } else if (arg.equals("-r") || arg.equals("--reset")) {
                resetStats = true;
            } else if (arg.equals("-h") || arg.equals("--help")) {
                System.out.println("Usage: typing [easy|medium|hard] [-t text] [-s] [-r]");
                return;
            }
        }
        if (resetStats) {
            String f = System.getProperty("user.home") + "/.typing_stats.json";
            try { Files.deleteIfExists(Paths.get(f)); } catch (Exception e) {}
            System.out.println("Статистика сброшена.");
            return;
        }
        if (showStats) {
            String f = System.getProperty("user.home") + "/.typing_stats.json";
            try {
                String json = new String(Files.readAllBytes(Paths.get(f)));
                // Упрощённый вывод
                System.out.println(colorize("📊 Статистика:", BOLD));
                json = json.trim();
                if (json.startsWith("{") && json.endsWith("}")) {
                    json = json.substring(1, json.length()-1);
                    String[] pairs = json.split(",");
                    for (String pair : pairs) {
                        String[] kv = pair.split(":");
                        if (kv.length == 2) {
                            String key = kv[0].trim().replaceAll("\"", "");
                            double val = Double.parseDouble(kv[1].trim());
                            System.out.printf("  %s: %.1f WPM\n", key, val);
                        }
                    }
                }
            } catch (Exception e) {
                System.out.println("Статистика пуста.");
            }
            return;
        }
        typing test = new typing(level);
        if (!customText.isEmpty()) {
            test.text = customText;
        }
        test.run();
    }
}
