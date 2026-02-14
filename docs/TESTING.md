# План и документация по тестированию

Документ описывает стратегию тестирования CLI Shell Interpreter, покрытие по компонентам и ожидаемое поведение (вход/выход) для ключевых тестов. На момент актуализации в наборе **около 190 тестов** (Google Test), запускаемых через `ctest` и в CI.

---

## 1. Цели тестирования

- **Покрытие API:** каждый публичный метод и тип по возможности покрыт юнит-тестом.
- **Прослеживаемость:** для каждого теста указано, что проверяется, какой вход и какой выход ожидается (где применимо).
- **Регрессии:** CI запускает все тесты на каждом push/PR.

---

## 2. Уровни тестов

| Уровень        | Файлы              | Что проверяем |
|----------------|--------------------|----------------|
| Юнит (модуль)  | test_token, test_environment, test_input_reader, test_lexer, test_parser, test_substitutor, test_parsed_command, test_commands, test_pipeline, test_executor | Один класс/функция, изолированно |
| Интеграция     | test_integration   | Цепочка: processLine → подстановка → парсинг → исполнение |
| Краевые случаи | test_edge_cases   | Кавычки, переменные, коды возврата, внешние команды, пайплайны, подстановки в пайпе, exit в пайпе, пустые команды в пайпе, устойчивость (shell не падает на ошибочном вводе) |

---

## 3. Покрытие по компонентам

### 3.1 Token (`include/shell/token.hpp`)

| Метод/функция           | Тест-файл      | Что проверяем | Вход | Выход |
|-------------------------|----------------|---------------|------|--------|
| `Token(type, value)`    | test_token.cpp | Создание токена | TokenType::WORD, "echo" | type==WORD, value=="echo" |
| `operator==`            | test_token.cpp | Равенство по type и value | Два токена | true/false |
| `operator!=`            | test_token.cpp | Отрицание равенства | Два токена | true/false |
| `tokenTypeToString()`   | test_token.cpp | Строковое представление типа | WORD, PIPE, ASSIGNMENT, END_OF_INPUT | "WORD", "PIPE", ... |

### 3.2 Environment (`include/shell/environment.hpp`)

| Метод              | Тест-файл           | Что проверяем | Вход | Выход |
|--------------------|---------------------|---------------|------|--------|
| `get(name)`        | test_environment.cpp | Получение значения; несуществующая → "" | name | value или "" |
| `set(name, value)` | test_environment.cpp | Установка; перезапись | name, value | — |
| `unset(name)`      | test_environment.cpp | Удаление; после unset get возвращает "" | name | — |
| `contains(name)`   | test_environment.cpp | Наличие переменной | name | true/false |
| `toEnvp()`         | test_environment.cpp | Формат "NAME=VALUE", порядок | — | vector<string> |
| `initFromSystem()` | test_environment.cpp | Не падает; после init есть переменные или "?" | — | — |

### 3.3 InputReader (`include/shell/input_reader.hpp`)

| Метод              | Тест-файл             | Что проверяем | Вход (поток) | Выход |
|--------------------|------------------------|---------------|--------------|--------|
| `readLine()`       | test_input_reader.cpp | Чтение строки до \n | "hello\n" | "hello" |
| `readLine()` EOF   | test_input_reader.cpp | При EOF — nullopt | (пустой поток) | nullopt |
| `setPrompt(prompt)`| test_input_reader.cpp | Смена приглашения | "$$ " | при следующем readLine используется новый prompt |
| `showPrompt(bool)` | test_input_reader.cpp | Вкл/выкл вывод приглашения | false | при readLine приглашение не пишется |

### 3.4 Lexer (`include/shell/lexer.hpp`)

| Сценарий | Тест-файл    | Что проверяем | Вход (строка) | Выход (токены) |
|----------|--------------|---------------|----------------|-----------------|
| Пустой ввод | test_lexer.cpp | Только END_OF_INPUT | "" | [END_OF_INPUT] |
| Одно слово | test_lexer.cpp | WORD + END_OF_INPUT | "echo" | [WORD("echo"), END_OF_INPUT] |
| Несколько слов | test_lexer.cpp | Разбиение по пробелам | "echo hello world" | [echo, hello, world, END] |
| Pipe | test_lexer.cpp | Токен PIPE | "cat \| wc" | [cat, file.txt, PIPE, wc, END] |
| Двойные кавычки | test_lexer.cpp | Один WORD с пробелами | "echo \"hello world\"" | [echo, "hello world", END] |
| Одинарные кавычки | test_lexer.cpp | Аналогично | "echo 'hello world'" | [echo, "hello world", END] |
| Пустая строка/пробелы | test_lexer.cpp | Только END_OF_INPUT | "   \t  " | [END_OF_INPUT] |

### 3.5 Parser (`include/shell/parser.hpp`)

| Сценарий | Тест-файл    | Что проверяем | Вход (токены) | Выход (AST) |
|----------|--------------|---------------|----------------|-------------|
| Пустая строка | test_parser.cpp | ParsedEmpty | []/только END | isEmpty()==true |
| Простая команда | test_parser.cpp | ParsedPipeline с одной ParsedSimpleCommand | echo hello | commandName=="echo", args==["hello"] |
| Пайплайн | test_parser.cpp | Несколько команд | cat \| wc | commands.size()==2 |
| Присваивание | test_parser.cpp | ParsedAssignment | FOO=bar | variableName=="FOO", value=="bar" |
| Список присваиваний | test_parser.cpp | ParsedAssignmentList | x=1 y=2 | assignments.size()==2 |

### 3.6 Substitutor (`include/shell/substitutor.hpp`)

| Сценарий | Тест-файл         | Что проверяем | Вход (строка + env) | Выход (строка) |
|----------|-------------------|---------------|----------------------|----------------|
| Без переменных | test_substitutor.cpp | Без изменений | "hello", {} | "hello" |
| $VAR | test_substitutor.cpp | Подстановка | "echo $FOO", FOO=bar | "echo bar" |
| ${VAR} | test_substitutor.cpp | Подстановка | "echo ${FOO}", FOO=bar | "echo bar" |
| В одинарных кавычках | test_substitutor.cpp | Не подставлять | "'$FOO'", FOO=bar | "'$FOO'" |
| Несуществующая переменная | test_substitutor.cpp | Пустая строка | "$UNDEF", {} | "" |

### 3.7 ParsedCommand и наследники (`include/shell/parsed_command.hpp`)

| Тип | Тест-файл             | Что проверяем | Вход | Выход |
|-----|------------------------|---------------|------|--------|
| ParsedEmpty | test_parsed_command.cpp | isEmpty() | — | true |
| ParsedAssignment | test_parsed_command.cpp | Конструктор, isAssignment() | name, value | variableName, value |
| ParsedSimpleCommand | test_parsed_command.cpp | Конструктор, commandName, arguments | name, args | — |
| ParsedPipeline | test_parsed_command.cpp | addCommand, isPipeline(), commands | addCommand(...) | size, commandName |
| ParsedAssignmentList | test_parsed_command.cpp | addAssignment, isAssignment() | addAssignment(...) | assignments.size() |

### 3.8 Command и реализации (`include/shell/command.hpp`, `commands/*`)

| Команда | Что проверяем | Вход (аргументы / поток) | Выход (код, stdout, stderr) |
|---------|----------------|---------------------------|------------------------------|
| EchoCommand | getName, setArguments, execute | args=["a","b"], in=пусто | 0, "a b\n", "" |
| CatCommand | Без аргументов — копия stdin | in="x", args=[] | 0, "x", "" |
| CatCommand | С аргументом — содержимое файла | args=["path"], in=игнор | 0, содержимое; или 1, "", сообщение в err |
| WcCommand | Подсчёт строк/слов/байт | in="a b\n", args=[] | 0, "1 2 4\n", "" |
| PwdCommand | Текущая директория | — | 0, путь с '/', "" |
| ExitCommand | wasExitRequested, getExitCode | args=[] / args=["42"] | 0/42, флаг true |
| ExternalCommand | Запуск по PATH, передача env | name="true", env | 0; name="/nonexistent" → не 0 |

Дополнительно (критерии ДЗ): `cat .gitignore`, `wc .gitignore` — тесты в test_commands или test_integration.

### 3.9 Pipeline, PipelineBuilder, Executor

| Компонент | Метод | Что проверяем | Вход | Выход |
|-----------|--------|---------------|------|--------|
| Pipeline | addCommand, size, isEmpty, getCommand | Добавление команд, доступ по индексу | addCommand(cmd) | size(), getCommand(i) |
| PipelineBuilder | build(parsed) | Построение из ParsedPipeline | ParsedPipeline с 2 командами | Pipeline size==2 |
| Executor | execute(pipeline) | Выполнение пайплайна, код возврата | Pipeline echo+wc | 0, stdout "1 2 12\n" |
| Executor | executeAssignment, executeAssignments | Запись в Environment | ParsedAssignment FOO=bar | env.get("FOO")=="bar" |
| Executor | shouldExit, getExitCode | После ExitCommand | pipeline с exit 42 | shouldExit()==true, getExitCode()==42 |

### 3.10 CommandFactory

| Метод | Что проверяем | Вход | Выход |
|-------|----------------|------|--------|
| create("echo") | Встроенная команда | "echo" | unique_ptr<EchoCommand> (getName()=="echo") |
| create("unknown") | Внешняя команда | "unknown" | unique_ptr<ExternalCommand> |
| isBuiltin("echo") | true | "echo", "cat", "wc", "pwd", "exit" | true |
| isBuiltin("ls") | false | "ls" | false |

### 3.11 Shell

| Метод | Что проверяем | Вход | Выход |
|-------|----------------|------|--------|
| processLine("echo x") | Полный цикл: подстановка → лексер → парсер → исполнение | строка | код возврата, stdout |
| processLine("") | Пустая строка | "" | 0 |
| getEnvironment() | Доступ к окружению (для тестов) | — | Environment& |

---

## 4. Запуск тестов

```bash
cd build
cmake .. -DBUILD_TESTING=ON
cmake --build .
ctest --output-on-failure
# или
./shell_tests
```

С санитайзерами (рекомендуется локально):

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON \
  -DENABLE_SANITIZER_ADDRESS=ON -DENABLE_SANITIZER_UNDEFINED_BEHAVIOR=ON
cmake --build .
ctest --output-on-failure
```

---

## 5. Соглашения по оформлению тестов

1. **Имя теста:** отражает сценарий, например `EchoSingleArg`, `SubstitutorUndefinedVariable`.
2. **Комментарий в начале теста (где нужно):** что проверяем, ожидаемый вход и выход.
3. **Fixture:** общая настройка (Environment, потоки) выносится в `SetUp()`.
4. **Изоляция:** тесты не зависят друг от друга; порядок запуска не важен.
5. **Файловая система:** тесты, создающие файлы (cat, wc по файлу), используют временные пути и удаляют файлы в конце (или /tmp).

---

## 6. Матрица тест-файлов и компонентов

| Файл                  | Компоненты |
|-----------------------|------------|
| test_token.cpp        | Token, tokenTypeToString |
| test_environment.cpp  | Environment |
| test_input_reader.cpp | InputReader |
| test_lexer.cpp        | Lexer |
| test_parser.cpp       | Parser |
| test_substitutor.cpp  | Substitutor |
| test_parsed_command.cpp | ParsedCommand, ParsedEmpty, ParsedAssignment, ParsedSimpleCommand, ParsedPipeline, ParsedAssignmentList |
| test_commands.cpp     | EchoCommand, CatCommand, WcCommand, PwdCommand, ExitCommand |
| test_pipeline.cpp     | Pipeline, PipelineBuilder, Executor, CommandFactory |
| test_executor.cpp     | Executor (детально: assignment, exit, пайплайн) |
| test_integration.cpp  | Shell.processLine (цепочка целиком) |
| test_edge_cases.cpp   | Кавычки, переменные, коды возврата, внешние команды, пайплайны, подстановки в пайпе, exit в пайпе, пустые команды в пайпе, устойчивость к ошибочному вводу |

---

## 7. Регрессии и CI

- В CI (`.github/workflows/ci.yml`) тесты запускаются на Ubuntu (GCC, Clang) и macOS (AppleClang).
- Отдельно собирается конфигурация с AddressSanitizer и UBSanitizer; тесты прогоняются с санитайзерами.
- При добавлении нового публичного API или сценария использования добавляется соответствующий тест и при необходимости обновляется этот документ.
