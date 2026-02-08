#!/bin/bash
# Скрипт установки git hooks
# Использование: ./scripts/install-hooks.sh

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
HOOKS_DIR="$PROJECT_DIR/.git/hooks"

echo "Установка git hooks..."

# Создаём директорию hooks если её нет
mkdir -p "$HOOKS_DIR"

# Удаляем старый pre-commit если есть
rm -f "$HOOKS_DIR/pre-commit"

# Копируем pre-push hook
cp "$SCRIPT_DIR/pre-push" "$HOOKS_DIR/pre-push"
chmod +x "$HOOKS_DIR/pre-push"
echo "✓ pre-push hook установлен"

echo ""
echo "Готово! Перед каждым пушем будет выполняться:"
echo "  1. Проверка форматирования (clang-format)"
echo "  2. Статический анализ (clang-tidy)"
echo "  3. Сборка проекта"
echo "  4. Запуск тестов (70 тестов)"
echo ""
echo "Для пропуска проверки: git push --no-verify"
