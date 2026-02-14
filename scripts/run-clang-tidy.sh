#!/bin/bash
# Скрипт для запуска clang-tidy на всём проекте
# Использование: ./scripts/run-clang-tidy.sh [--fix]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

FIX_FLAG=""
if [ "$1" = "--fix" ]; then
    FIX_FLAG="--fix"
    echo "Режим автоисправления включён"
fi

# Проверяем наличие clang-tidy
if ! command -v clang-tidy &> /dev/null; then
    echo "❌ clang-tidy не найден. Установите его:"
    echo "   macOS: brew install llvm"
    echo "   Ubuntu: sudo apt install clang-tidy"
    exit 1
fi

# Проверяем/создаём compile_commands.json
if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
    echo "Генерация compile_commands.json..."
    cmake -B "$BUILD_DIR" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON "$PROJECT_DIR" > /dev/null
fi

echo "Запуск clang-tidy..."
echo ""

# Собираем список файлов
FILES=$(find "$PROJECT_DIR/src" "$PROJECT_DIR/include" -name "*.cpp" -o -name "*.hpp" 2>/dev/null)

ERRORS=0
WARNINGS=0

for FILE in $FILES; do
    REL_PATH="${FILE#$PROJECT_DIR/}"
    OUTPUT=$(clang-tidy -p "$BUILD_DIR" $FIX_FLAG "$FILE" 2>&1 || true)
    
    if echo "$OUTPUT" | grep -q "error:"; then
        echo "❌ $REL_PATH"
        echo "$OUTPUT" | grep -E "(error:|warning:)" | sed 's/^/   /'
        ERRORS=$((ERRORS + 1))
    elif echo "$OUTPUT" | grep -q "warning:"; then
        echo "⚠️  $REL_PATH"
        echo "$OUTPUT" | grep "warning:" | head -3 | sed 's/^/   /'
        WARNINGS=$((WARNINGS + 1))
    else
        echo "✓ $REL_PATH"
    fi
done

echo ""
echo "================================"
echo "Результаты clang-tidy:"
echo "  Файлов с ошибками: $ERRORS"
echo "  Файлов с предупреждениями: $WARNINGS"
echo "================================"

if [ $ERRORS -gt 0 ]; then
    exit 1
fi

exit 0
