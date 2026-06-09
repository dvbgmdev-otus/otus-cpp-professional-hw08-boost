/**
 * @file name_matcher_test.cpp
 * @brief Тесты проверки имён файлов по маскам.
 */

#include "name_matcher.h"

#include <gtest/gtest.h>

#if (1)  // Группа 1. Пустой список масок
// 1.1. Пустой список масок разрешает любое имя файла.
TEST(NameMatcherEmptyMasks, AnyFileNameShouldMatch) {
    const NameMatcher matcher({});
    EXPECT_TRUE(matcher.matches("report.txt"));
    EXPECT_TRUE(matcher.matches("archive.bin"));
}
#endif

#if (1)  // Группа 2. Сравнение без учёта регистра
// 2.1. Маска и имя файла в разном регистре считаются совпадающими.
TEST(NameMatcherCaseInsensitive, SameNameWithDifferentCaseShouldMatch) {
    const NameMatcher matcher({ "*.TXT" });
    EXPECT_TRUE(matcher.matches("report.txt"));
    EXPECT_TRUE(matcher.matches("README.TXT"));
}
#endif

#if (1)  // Группа 3. Wildcard-маски
// 3.1. Символ '*' соответствует любой последовательности символов.
TEST(NameMatcherWildcard, StarShouldMatchAnySequence) {
    const NameMatcher matcher({ "log_*.txt" });
    EXPECT_TRUE(matcher.matches("log_2024.txt"));
    EXPECT_TRUE(matcher.matches("log_.txt"));
    EXPECT_FALSE(matcher.matches("report_2024.txt"));
}

// 3.2. Символ '?' соответствует одному произвольному символу.
TEST(NameMatcherWildcard, QuestionShouldMatchSingleCharacter) {
    const NameMatcher matcher({ "file_?.dat" });
    EXPECT_TRUE(matcher.matches("file_1.dat"));
    EXPECT_FALSE(matcher.matches("file_10.dat"));
    EXPECT_FALSE(matcher.matches("file_.dat"));
}

// 3.3. Хвостовой символ '*' может соответствовать пустой последовательности.
TEST(NameMatcherWildcard, TrailingStarShouldMatchEmptySequence) {
    const NameMatcher matcher({ "file*" });
    EXPECT_TRUE(matcher.matches("file"));
}
#endif

#if (1)  // Группа 4. Проверка только имени файла
// 4.1. Маска применяется к имени файла, а не к полному пути.
TEST(NameMatcherFileNameOnly, DirectoryNameShouldNotAffectMatch) {
    const NameMatcher matcher({ "*.cpp" });
    EXPECT_TRUE(matcher.matches("/tmp/project/src/main.cpp"));
    EXPECT_FALSE(matcher.matches("/tmp/project.cpp/main.txt"));
}
#endif
