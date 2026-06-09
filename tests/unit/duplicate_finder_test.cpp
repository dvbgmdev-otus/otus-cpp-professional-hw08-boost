/**
 * @file duplicate_finder_test.cpp
 * @brief Тесты поиска файлов-дубликатов.
 */

#include "duplicate_finder.h"

#include <boost/filesystem.hpp>

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {

namespace fs = boost::filesystem;

class DuplicateFinderTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_root = fs::temp_directory_path() / fs::unique_path("bayan_duplicate_finder_%%%%-%%%%-%%%%");
        fs::create_directories(m_root);
        m_hasher = create_hasher(HashAlgorithm::Crc32);
    }

    void TearDown() override {
        boost::system::error_code error;
        fs::remove_all(m_root, error);
    }

    FileInfo make_file(const std::string& name, const std::string& content) const {
        const fs::path path = m_root / name;
        std::ofstream file(path.string(), std::ios::binary);
        file << content;
        file.close();

        return { path, static_cast<std::uintmax_t>(content.size()) };
    }

    static std::vector<fs::path> sorted_group(std::vector<fs::path> group) {
        std::sort(group.begin(), group.end());
        return group;
    }

    fs::path m_root;
    std::unique_ptr<IBlockHasher> m_hasher;
};

}  // namespace

#if (1)  // Группа 1. Базовые сценарии поиска
// 1.1. Одинаковые файлы попадают в одну группу дубликатов.
TEST_F(DuplicateFinderTest, SameFilesShouldBeReturnedAsDuplicateGroup) {
    const FileInfo first = make_file("first.txt", "abcdef");
    const FileInfo second = make_file("second.txt", "abcdef");
    DuplicateFinder finder(3, *m_hasher);

    const std::vector<std::vector<fs::path>> duplicates = finder.find({ first, second });

    ASSERT_EQ(duplicates.size(), 1U);
    EXPECT_EQ(sorted_group(duplicates.front()), sorted_group({ first.path, second.path }));
}

// 1.2. Три одинаковых файла попадают в одну группу дубликатов.
TEST_F(DuplicateFinderTest, ThreeSameFilesShouldBeReturnedAsOneDuplicateGroup) {
    const FileInfo first = make_file("first.txt", "abcdef");
    const FileInfo second = make_file("second.txt", "abcdef");
    const FileInfo third = make_file("third.txt", "abcdef");
    DuplicateFinder finder(3, *m_hasher);

    const std::vector<std::vector<fs::path>> duplicates = finder.find({ first, second, third });

    ASSERT_EQ(duplicates.size(), 1U);
    EXPECT_EQ(sorted_group(duplicates.front()), sorted_group({ first.path, second.path, third.path }));
}
#endif

#if (1)  // Группа 2. Отбрасывание разных файлов
// 2.1. Файлы разного размера не считаются дубликатами.
TEST_F(DuplicateFinderTest, DifferentSizeFilesShouldNotBeReturned) {
    const FileInfo first = make_file("first.txt", "abc");
    const FileInfo second = make_file("second.txt", "abcd");
    DuplicateFinder finder(3, *m_hasher);

    EXPECT_TRUE(finder.find({ first, second }).empty());
}

// 2.2. Файлы одинакового размера с разным содержимым не считаются дубликатами.
TEST_F(DuplicateFinderTest, SameSizeDifferentContentFilesShouldNotBeReturned) {
    const FileInfo first = make_file("first.txt", "abcdef");
    const FileInfo second = make_file("second.txt", "abcdeg");
    DuplicateFinder finder(3, *m_hasher);

    EXPECT_TRUE(finder.find({ first, second }).empty());
}
#endif

#if (1)  // Группа 3. Разные группы дубликатов
// 3.1. Разные наборы одинаковых файлов возвращаются отдельными группами.
TEST_F(DuplicateFinderTest, DifferentDuplicateSetsShouldBeReturnedAsSeparateGroups) {
    const FileInfo first_a = make_file("first_a.txt", "abcdef");
    const FileInfo first_b = make_file("first_b.txt", "abcdef");
    const FileInfo second_a = make_file("second_a.txt", "123456");
    const FileInfo second_b = make_file("second_b.txt", "123456");
    DuplicateFinder finder(3, *m_hasher);

    std::vector<std::vector<fs::path>> duplicates = finder.find({ first_a, first_b, second_a, second_b });
    for (std::vector<fs::path>& group : duplicates) {
        group = sorted_group(group);
    }
    std::sort(duplicates.begin(), duplicates.end());

    std::vector<std::vector<fs::path>> expected = {
        sorted_group({ first_a.path, first_b.path }),
        sorted_group({ second_a.path, second_b.path })
    };
    std::sort(expected.begin(), expected.end());

    EXPECT_EQ(duplicates, expected);
}
#endif

#if (1)  // Группа 4. Ошибки конфигурации
// 4.1. Нулевой размер блока запрещён.
TEST_F(DuplicateFinderTest, ZeroBlockSizeShouldThrow) {
    EXPECT_THROW(DuplicateFinder(0, *m_hasher), std::invalid_argument);
}
#endif
