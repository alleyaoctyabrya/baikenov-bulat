# Battleship Game 🛳️

## Содержание

1. Требования
2. Сборка и запуск игры
3. Запуск тестов
4. Структура проекта

## Требования

| Инструмент                                  | Минимальная версия           |
| ------------------------------------------- | ---------------------------- |
| **CMake**                                   | 3.15                         |
| **C++17-совместимый компилятор**            | GCC 9 / Clang 10 / MSVC 2019 |
| **Git** (для *FetchContent*)                | любой                        |

## Сборка и запуск игры

```bash
git clone https://github.com/your-name/BattleshipGame.git
cd BattleshipGame

cmake -S . -B build

cmake --build build

./build/BattleshipGame
```

## Запуск тестов


```bash
cmake --build build --target test
```

## Структура проекта

```
BattleshipGame/
├── CMakeLists.txt          # единая точка сборки
├── include/                # заголовки
├── src/                    # исходники игры
├── test/                   # *_test.cpp (Google Test)
└── res/                    # ресурсы (шрифты, текстуры, звуки)
```

