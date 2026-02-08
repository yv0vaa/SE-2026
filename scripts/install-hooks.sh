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

echo "✓ pre-commit hook установлен"
echo ""
echo "Теперь перед каждым коммитом C++ файлы будут автоматически"
echo "отформатированы с помощью clang-format."
