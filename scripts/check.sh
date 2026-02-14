#!/bin/bash
# Скрипт для локальной проверки проекта перед пушем
# Использование: ./scripts/check.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build-check"

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== Локальная проверка проекта ===${NC}"
echo ""

# Очистка и создание директории сборки
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Конфигурация
echo -e "${YELLOW}[1/4] Конфигурация CMake...${NC}"
cmake "$PROJECT_DIR" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON \
    -DCMAKE_CXX_COMPILER=c++ \
    > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Ошибка конфигурации CMake${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Конфигурация успешна${NC}"

# Сборка
echo -e "${YELLOW}[2/4] Сборка проекта...${NC}"
if ! cmake --build . --parallel 2>&1; then
    echo -e "${RED}❌ Ошибка сборки${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Сборка успешна${NC}"

# Тесты
echo -e "${YELLOW}[3/4] Запуск тестов...${NC}"
if ! ctest --output-on-failure; then
    echo -e "${RED}❌ Тесты не прошли${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Все тесты пройдены${NC}"

# Проверка форматирования (опционально)
echo -e "${YELLOW}[4/4] Проверка форматирования...${NC}"
if command -v clang-format &> /dev/null; then
    cd "$PROJECT_DIR"
    FORMAT_ISSUES=$(find src include tests -name "*.cpp" -o -name "*.hpp" | xargs clang-format --dry-run --Werror 2>&1 || true)
    if [ -n "$FORMAT_ISSUES" ]; then
        echo -e "${YELLOW}⚠ Есть замечания по форматированию (не критично)${NC}"
    else
        echo -e "${GREEN}✓ Форматирование в порядке${NC}"
    fi
else
    echo -e "${YELLOW}⚠ clang-format не найден, пропускаем${NC}"
fi

# Очистка
rm -rf "$BUILD_DIR"

echo ""
echo -e "${GREEN}=== Все проверки пройдены! Можно пушить ===${NC}"
