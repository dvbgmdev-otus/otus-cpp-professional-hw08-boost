/**
 * @file file_signature_test.cpp
 * @brief Тесты ленивой сигнатуры файла.
 */

#include "file_signature.h"

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

namespace fs = boost::filesystem;

class RecordingHasher final : public IBlockHasher {
public:
    std::string hash(const std::vector<char>& block) const override {
        m_blocks.push_back(block);
        return std::string("hash_") + std::to_string(m_blocks.size());
    }

    mutable std::vector<std::vector<char>> m_blocks;
};

class FileSignatureTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_root = fs::temp_directory_path() / fs::unique_path("bayan_file_signature_%%%%-%%%%-%%%%");
        fs::create_directories(m_root);
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

    fs::path m_root;
    RecordingHasher m_hasher;
};

}  // namespace

#if (1)  // Группа 1. Метаданные сигнатуры
// 1.1. Сигнатура возвращает путь и размер исходного файла.
TEST_F(FileSignatureTest, MetadataShouldMatchFileInfo) {
    const FileInfo file = make_file("data.txt", "content");
    FileSignature signature(file, 4, m_hasher);
    EXPECT_EQ(signature.path(), file.path);
    EXPECT_EQ(signature.size(), file.size);
}

// 1.2. Количество блоков округляется вверх.
TEST_F(FileSignatureTest, BlockCountShouldRoundUp) {
    FileSignature signature(make_file("data.txt", "12345"), 2, m_hasher);
    EXPECT_EQ(signature.block_count(), 3U);
}

// 1.3. Пустой файл не содержит блоков.
TEST_F(FileSignatureTest, EmptyFileShouldHaveNoBlocks) {
    FileSignature signature(make_file("empty.txt", ""), 4, m_hasher);
    EXPECT_EQ(signature.block_count(), 0U);
}
#endif

#if (1)  // Группа 2. Чтение и дополнение блоков
// 2.1. Запрошенный блок читается из файла и хэшируется.
TEST_F(FileSignatureTest, BlockHashShouldHashRequestedBlock) {
    FileSignature signature(make_file("data.txt", "abcdef"), 3, m_hasher);
    EXPECT_EQ(signature.block_hash(1), "hash_1");
    EXPECT_EQ(m_hasher.m_blocks, std::vector<std::vector<char>>({ { 'd', 'e', 'f' } }));
}

// 2.2. Последний неполный блок дополняется нулями.
TEST_F(FileSignatureTest, LastBlockShouldBePaddedWithZeroes) {
    FileSignature signature(make_file("data.txt", "abcde"), 4, m_hasher);
    EXPECT_EQ(signature.block_hash(1), "hash_1");
    EXPECT_EQ(m_hasher.m_blocks, std::vector<std::vector<char>>({ { 'e', '\0', '\0', '\0' } }));
}
#endif

#if (1)  // Группа 3. Кэширование хэшей
// 3.1. Повторный запрос блока возвращает кэшированный хэш.
TEST_F(FileSignatureTest, RepeatedBlockHashShouldUseCache) {
    FileSignature signature(make_file("data.txt", "abcdef"), 3, m_hasher);
    EXPECT_EQ(signature.block_hash(0), "hash_1");
    EXPECT_EQ(signature.block_hash(0), "hash_1");
    EXPECT_EQ(m_hasher.m_blocks.size(), 1U);
}
#endif

#if (1)  // Группа 4. Ошибки
// 4.1. Нулевой размер блока запрещён.
TEST_F(FileSignatureTest, ZeroBlockSizeShouldThrow) {
    EXPECT_THROW(FileSignature(make_file("data.txt", "abc"), 0, m_hasher), std::invalid_argument);
}

// 4.2. Запрос несуществующего блока запрещён.
TEST_F(FileSignatureTest, OutOfRangeBlockShouldThrow) {
    FileSignature signature(make_file("data.txt", "abc"), 3, m_hasher);

    EXPECT_THROW(signature.block_hash(1), std::out_of_range);
}

// 4.3. Ошибка открытия файла приводит к исключению.
TEST_F(FileSignatureTest, MissingFileShouldThrow) {
    const FileInfo file{ m_root / "missing.txt", 1 };
    FileSignature signature(file, 1, m_hasher);

    EXPECT_THROW(signature.block_hash(0), std::runtime_error);
}
#endif
