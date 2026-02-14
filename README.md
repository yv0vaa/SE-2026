# CLI Shell Interpreter

[![CI](https://github.com/yv0vaa/SE-2026/actions/workflows/ci.yml/badge.svg)](https://github.com/yv0vaa/SE-2026/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Интерпретатор командной оболочки на C++ с поддержкой пайплайнов, переменных окружения и встроенных команд.

## Описание проекта

CLI Shell Interpreter — это учебная реализация командной оболочки (shell), демонстрирующая основные принципы работы интерпретаторов командной строки. Проект разработан в рамках курса программной инженерии.

### Возможности

- **Встроенные команды**: `cat`, `echo`, `wc`, `pwd`, `exit`
- **Пайплайны**: объединение команд через `|` (например, `cat file.txt | wc`)
- **Переменные окружения**: чтение и установка переменных (`$VAR`, `VAR=value`)
- **Внешние программы**: запуск любых исполняемых файлов системы
- **Кавычки**: поддержка одинарных и двойных кавычек

## Архитектура

Подробная архитектурная документация находится в файле [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md).

### Краткий обзор компонентов

```
┌──────────────────────────────────────────────────────────────────┐
│                         Shell (main)                             │
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │                      REPL Loop                              │ │
│  │                                                             │ │
│  │   ┌──────────┐    ┌────────────┐    ┌──────────────────┐    │ │
│  │   │  Input   │──▶│ Substitutor │──▶│     Lexer        │    │ │
│  │   │  Reader  │    │            │    │                  │    │ │
│  │   └──────────┘    └────────────┘    └────────┬─────────┘    │ │
│  │                                              │              │ │
│  │   ┌──────────┐    ┌────────────┐    ┌───────▼──────────┐    │ │
│  │   │ Executor │◀──│  Pipeline  │◀── │     Parser       │    │ │
│  │   │          │    │  Builder   │    │                  │    │ │
│  │   └──────────┘    └────────────┘    └──────────────────┘    │ │
│  └─────────────────────────────────────────────────────────────┘ │
└──────────────────────────────────────────────────────────────────┘
```

## Пример использования

```bash
# Запуск интерпретатора
$ ./shell

# Пример сессии
> echo Hello, World!
Hello, World!

> FOO=bar
> echo $FOO
bar

> cat README.md | wc
  42  156  1234

> pwd
/home/user/project

> exit
```

## Сборка и запуск

### Требования

- C++17 или новее
- CMake 3.16+
- Компилятор с поддержкой C++17 (GCC 9+, Clang 10+, MSVC 2019+)

### Сборка

```bash
# Клонирование репозитория
git clone https://github.com/yv0vaa/SE-2026.git
cd SE-2026

# Создание директории для сборки
mkdir build && cd build

# Конфигурация и сборка
cmake .. -DBUILD_TESTING=ON
cmake --build .

# Запуск тестов (70 тестов)
ctest --output-on-failure

# Запуск интерпретатора
./shell
```

### Сборка с санитайзерами (рекомендуется для разработки)

```bash
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DBUILD_TESTING=ON \
      -DENABLE_SANITIZER_ADDRESS=ON \
      -DENABLE_SANITIZER_UNDEFINED_BEHAVIOR=ON \
      ..
cmake --build .
```

## Настройка окружения разработчика

### Установка git-хуков

Перед началом работы установите git-хуки для автоматических проверок:

```bash
./scripts/install-hooks.sh
```

Это установит **pre-push hook**, который перед каждым пушем:
1. ✅ Проверяет и автоматически форматирует код (`clang-format`)
2. ✅ Запускает статический анализ (`clang-tidy`)
3. ✅ Собирает проект
4. ✅ Запускает все 70 тестов

Для пропуска проверок (не рекомендуется):
```bash
git push --no-verify           # пропустить всё
SKIP_FORMAT=1 git push         # пропустить форматирование
SKIP_TIDY=1 git push           # пропустить clang-tidy
```

### Рекомендуемые инструменты

- `clang-format` — автоформатирование кода
- `clang-tidy` — статический анализ
- `cmake` 3.16+

На macOS:
```bash
brew install cmake llvm
```

На Ubuntu:
```bash
sudo apt install cmake clang-format clang-tidy
```

## Структура проекта

```
SE-2026/
├── docs/
│   └── ARCHITECTURE.md       # Архитектурная документация
├── include/
│   └── shell/                # Заголовочные файлы
│       └── commands/         # Команды
├── src/
│   └── shell/                # Исходный код
│       └── commands/         # Реализации команд
├── tests/                    # Модульные тесты (70 тестов)
├── scripts/
│   ├── install-hooks.sh      # Установка git-хуков
│   ├── pre-push              # Pre-push hook
│   └── check.sh              # Локальный скрипт проверки
├── .github/
│   └── workflows/
│       └── ci.yml            # CI конфигурация
├── CMakeLists.txt            # Конфигурация сборки
├── .clang-format             # Настройки форматирования
├── .clang-tidy               # Настройки статического анализа
└── README.md
```

## Запуск тестов

```bash
cd build

# Через CTest
ctest --output-on-failure

# Или напрямую
./shell_tests
```

План тестирования, описание покрытия по компонентам и соглашения (вход/выход для тестов) описаны в [docs/TESTING.md](docs/TESTING.md).

## Стиль кодирования

Проект следует [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) с некоторыми исключениями:

- Отступы: 4 пробела
- Максимальная длина строки: 100 символов

Для автоматического форматирования используется `clang-format`:

```bash
# Форматирование всех файлов
find src include tests -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

## Лицензия

Проект распространяется под лицензией MIT. См. файл [LICENSE](LICENSE) для деталей.

## Авторы

- [Захаров Владимир](https://github.com/yv0vaa)
- [Никоненко Егор](https://github.com/animeemorave)
- [Шатохин Лев](https://github.com/TrueKotenka)

## Как помочь проекту

1. Форкните репозиторий
2. Создайте ветку для вашей функции (`git checkout -b feature/amazing-feature`)
3. Закоммитьте изменения (`git commit -m 'feat: add amazing feature'`)
4. Запушьте в ветку (`git push origin feature/amazing-feature`)
5. Откройте Pull Request

### Соглашения о коммитах

Проект следует [Conventional Commits](https://www.conventionalcommits.org/):

- `feat:` — новая функциональность
- `fix:` — исправление ошибки
- `docs:` — изменения в документации
- `test:` — добавление или изменение тестов
- `refactor:` — рефакторинг кода
- `style:` — форматирование, отступы
- `chore:` — обслуживание проекта
