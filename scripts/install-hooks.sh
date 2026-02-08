#!/bin/bash
# Скрипт установки git hooks
# Использование: ./scripts/install-hooks.sh

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
HOOKS_DIR="$PROJECT_DIR/.git/hooks"

echo "Установка git hooks..."

# Создаём директорию hooks если её нет
mkdir -p "$HOOKS_DIR"

# Копируем pre-commit hook
cp "$SCRIPT_DIR/pre-commit" "$HOOKS_DIR/pre-commit"
chmod +x "$HOOKS_DIR/pre-commit"
echo "✓ pre-commit hook установлен (clang-format + clang-tidy)"

# Копируем pre-push hook
cp "$SCRIPT_DIR/pre-push" "$HOOKS_DIR/pre-push"
chmod +x "$HOOKS_DIR/pre-push"
echo "✓ pre-push hook установлен (полная сборка + тесты)"

echo ""
echo "Готово! Теперь:"
echo "  • Перед коммитом: автоформатирование + clang-tidy"
echo "  • Перед пушем: полная сборка + 70 тестов"
