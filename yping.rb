#!/usr/bin/env ruby
# typing.rb
# encoding: UTF-8

require 'json'
require 'fileutils'
require 'timeout'

COLORS = {
  reset: "\e[0m",
  green: "\e[92m",
  red: "\e[91m",
  yellow: "\e[93m",
  blue: "\e[94m",
  bold: "\e[1m"
}

def colorize(text, color)
  "#{COLORS[color]}#{text}#{COLORS[:reset]}"
end

TEXTS = {
  'easy' => [
    'кот и собака играют во дворе',
    'солнце светит ярко сегодня',
    'быстрый лис прыгает через забор'
  ],
  'medium' => [
    'программирование это искусство создавать алгоритмы для решения задач',
    'в мире информационных технологий важно постоянно учиться новому',
    'компьютерные науки охватывают множество направлений'
  ],
  'hard' => [
    'разработка программного обеспечения требует глубоких знаний и навыков работы в команде',
    'современные технологии изменяют мир и открывают новые возможности для человечества',
    'искусственный интеллект и машинное обучение становятся основой будущих инноваций'
  ]
}

class TypingTest
  attr_reader :level, :text, :user_input, :start_time, :end_time, :stats_file, :stats

  def initialize(level = 'medium')
    @level = level
    @text = TEXTS[level].sample
    @stats_file = File.join(Dir.home, '.typing_stats.json')
    load_stats
  end

  def load_stats
    if File.exist?(@stats_file)
      @stats = JSON.parse(File.read(@stats_file))
    else
      @stats = {}
    end
  end

  def save_stats
    File.write(@stats_file, JSON.pretty_generate(@stats))
  end

  def display_text
    puts colorize("\nНапечатайте следующий текст:", :bold)
    puts colorize(@text, :blue)
    puts "\nНажмите Enter после ввода. Время начнётся с первой буквы."
    print "Готовы? Нажмите Enter..."
    gets
    puts "\nНачинайте печатать:"
  end

  def get_user_input
    @start_time = Time.now
    @user_input = gets.chomp
    @end_time = Time.now
  end

  def calculate_and_display
    original = @text
    typed = @user_input || ''
    total = original.length
    typed_len = typed.length
    correct = 0
    errors = 0
    original.chars.each_with_index do |ch, i|
      if i < typed_len && ch == typed[i]
        correct += 1
      else
        errors += 1
      end
    end
    if typed_len < total
      errors += total - typed_len
    end
    elapsed = @end_time - @start_time
    minutes = elapsed / 60.0
    wpm = (correct / 5.0) / minutes
    accuracy = (correct / total.to_f) * 100.0

    puts colorize("\n📊 Результаты:", :bold)
    puts "  Скорость: #{wpm.round(1)} WPM"
    puts "  Точность: #{accuracy.round(1)}%"
    puts "  Правильных символов: #{correct}/#{total}"
    puts "  Ошибок: #{errors}"
    puts "  Время: #{elapsed.round(2)} сек"

    if !@stats[@level] || wpm > @stats[@level]
      @stats[@level] = wpm
      save_stats
      puts colorize("🏆 Новый рекорд для этого уровня!", :green)
    else
      puts "Лучший результат: #{@stats[@level].round(1)} WPM"
    end
  end

  def run
    puts colorize("⌨️  Тест скорости печати", :bold)
    puts "Уровень: #{@level}"
    display_text
    get_user_input
    calculate_and_display
  end
end

def main
  level = 'medium'
  show_stats = false
  reset_stats = false
  custom_text = ''
  i = 0
  while i < ARGV.size
    arg = ARGV[i]
    case arg
    when 'easy', 'medium', 'hard'
      level = arg
    when '-t', '--text'
      custom_text = ARGV[i+1] if i+1 < ARGV.size
      i += 1
    when '-s', '--stats'
      show_stats = true
    when '-r', '--reset'
      reset_stats = true
    when '-h', '--help'
      puts "Usage: ruby typing.rb [easy|medium|hard] [-t text] [-s] [-r]"
      return
    end
    i += 1
  end
  if reset_stats
    f = File.join(Dir.home, '.typing_stats.json')
    File.delete(f) if File.exist?(f)
    puts 'Статистика сброшена.'
    return
  end
  if show_stats
    f = File.join(Dir.home, '.typing_stats.json')
    if File.exist?(f)
      stats = JSON.parse(File.read(f))
      puts colorize('📊 Статистика:', :bold)
      stats.each do |lvl, wpm|
        puts "  #{lvl}: #{wpm.round(1)} WPM"
      end
    else
      puts 'Статистика пуста.'
    end
    return
  end
  test = TypingTest.new(level)
  test.text = custom_text unless custom_text.empty?
  test.run
end

main if __FILE__ == $0
