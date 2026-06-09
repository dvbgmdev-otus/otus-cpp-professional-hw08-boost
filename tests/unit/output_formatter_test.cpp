/**
 * @file output_formatter_test.cpp
 * @brief Тесты форматирования групп файлов-дубликатов.
 */

#include "output_formatter.h"

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#if (1)  // Группа 1. Формат вывода
// 1.1. Пустой список групп не выводит данные.
TEST(OutputFormatter, EmptyGroupsShouldPrintNothing) {
    std::ostringstream output;
    print_duplicate_groups({}, output);
    EXPECT_EQ(output.str(), "");
}

// 1.2. Одна группа выводится по одному пути на строку.
TEST(OutputFormatter, SingleGroupShouldPrintOnePathPerLine) {
    std::ostringstream output;
    print_duplicate_groups({ { "/tmp/a.txt", "/tmp/b.txt" } }, output);
    EXPECT_EQ(output.str(), "/tmp/a.txt\n/tmp/b.txt\n");
}

// 1.3. Разные группы разделяются пустой строкой.
TEST(OutputFormatter, MultipleGroupsShouldBeSeparatedByEmptyLine) {
    std::ostringstream output;
    print_duplicate_groups({ { "/tmp/a.txt", "/tmp/b.txt" }, { "/tmp/c.txt", "/tmp/d.txt" } },
                           output);
    EXPECT_EQ(output.str(), "/tmp/a.txt\n/tmp/b.txt\n\n/tmp/c.txt\n/tmp/d.txt\n");
}
#endif
