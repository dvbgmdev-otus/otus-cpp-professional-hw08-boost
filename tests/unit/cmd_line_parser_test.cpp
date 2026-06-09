/**
 * @file cmd_line_parser_test.cpp
 * @brief Тесты разбора параметров командной строки.
 */

#include "cmd_line_parser.h"

#include <gtest/gtest.h>

#include <boost/filesystem/path.hpp>
#include <stdexcept>
#include <vector>

namespace {

CmdLineParseResult parse(const std::vector<const char*>& args) {
    const CmdLineParser parser;
    return parser.parse(static_cast<int>(args.size()), args.data());
}

}  // namespace

#if (1)  // Группа 1. Значения по умолчанию
// 1.1. Парсер заполняет настройки значениями по умолчанию.
TEST(CmdLineParserDefaults, EmptyArgumentsShouldUseDefaults) {
    const CmdLineParseResult result = parse({ "bayan" });
    EXPECT_FALSE(result.help_requested);
    EXPECT_EQ(result.config.include_dirs, std::vector<boost::filesystem::path>({ "." }));
    EXPECT_TRUE(result.config.exclude_dirs.empty());
    EXPECT_EQ(result.config.scan_depth, 0U);
    EXPECT_EQ(result.config.min_file_size, 1U);
    EXPECT_TRUE(result.config.masks.empty());
    EXPECT_EQ(result.config.block_size, 4096U);
    EXPECT_EQ(result.config.hash_algorithm, HashAlgorithm::Crc32);
}
#endif

#if (1)  // Группа 2. Списковые параметры
// 2.1. Парсер принимает несколько директорий сканирования.
TEST(CmdLineParserLists, MultipleIncludeDirectoriesShouldBeParsed) {
    const CmdLineParseResult result = parse({ "bayan", "--include", "src", "tests" });
    EXPECT_EQ(result.config.include_dirs, std::vector<boost::filesystem::path>({ "src", "tests" }));
}

// 2.2. Парсер принимает несколько директорий исключения.
TEST(CmdLineParserLists, MultipleExcludeDirectoriesShouldBeParsed) {
    const CmdLineParseResult result = parse({ "bayan", "--exclude", "build", "tmp" });
    EXPECT_EQ(result.config.exclude_dirs, std::vector<boost::filesystem::path>({ "build", "tmp" }));
}

// 2.3. Парсер принимает несколько масок файлов.
TEST(CmdLineParserLists, MultipleMasksShouldBeParsed) {
    const CmdLineParseResult result = parse({ "bayan", "--mask", "*.cpp", "*.h" });
    EXPECT_EQ(result.config.masks, std::vector<std::string>({ "*.cpp", "*.h" }));
}
#endif

#if (1)  // Группа 3. Числовые параметры
// 3.1. Парсер принимает уровень сканирования.
TEST(CmdLineParserNumbers, ScanDepthShouldBeParsed) {
    const CmdLineParseResult result = parse({ "bayan", "--level", "2" });
    EXPECT_EQ(result.config.scan_depth, 2U);
}

// 3.2. Парсер принимает минимальный размер файла.
TEST(CmdLineParserNumbers, MinFileSizeShouldBeParsed) {
    const CmdLineParseResult result = parse({ "bayan", "--min-size", "128" });
    EXPECT_EQ(result.config.min_file_size, 128U);
}

// 3.3. Парсер принимает размер блока.
TEST(CmdLineParserNumbers, BlockSizeShouldBeParsed) {
    const CmdLineParseResult result = parse({ "bayan", "--block-size", "8192" });
    EXPECT_EQ(result.config.block_size, 8192U);
}

// 3.4. Нулевой размер блока запрещён.
TEST(CmdLineParserNumbers, ZeroBlockSizeShouldThrow) {
    EXPECT_THROW(parse({ "bayan", "--block-size", "0" }), std::invalid_argument);
}
#endif

#if (1)  // Группа 4. Алгоритм хэширования
// 4.1. Парсер принимает алгоритм crc32.
TEST(CmdLineParserHash, Crc32ShouldBeParsed) {
    const CmdLineParseResult result = parse({ "bayan", "--hash", "crc32" });
    EXPECT_EQ(result.config.hash_algorithm, HashAlgorithm::Crc32);
}

// 4.2. Парсер принимает алгоритм md5.
TEST(CmdLineParserHash, Md5ShouldBeParsed) {
    const CmdLineParseResult result = parse({ "bayan", "--hash", "md5" });
    EXPECT_EQ(result.config.hash_algorithm, HashAlgorithm::Md5);
}

// 4.3. Неизвестный алгоритм хэширования запрещён.
TEST(CmdLineParserHash, UnknownHashShouldThrow) {
    EXPECT_THROW(parse({ "bayan", "--hash", "sha1" }), std::invalid_argument);
}
#endif

#if (1)  // Группа 5. Справка
// 5.1. Параметр help помечает запрос справки.
TEST(CmdLineParserHelp, HelpShouldBeDetected) {
    const CmdLineParseResult result = parse({ "bayan", "--help" });
    EXPECT_TRUE(result.help_requested);
    EXPECT_FALSE(result.help_message.empty());
}
#endif
