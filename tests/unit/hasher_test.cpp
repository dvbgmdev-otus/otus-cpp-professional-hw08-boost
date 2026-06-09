/**
 * @file hasher_test.cpp
 * @brief Тесты хэширования блоков данных.
 */

#include "hasher.h"

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<char> block_from(const std::string& value) {
    return std::vector<char>(value.begin(), value.end());
}

}  // namespace

#if (1)  // Группа 1. CRC32
// 1.1. CRC32 возвращает ожидаемый хэш для известного блока.
TEST(HasherCrc32, KnownBlockShouldReturnExpectedHash) {
    const std::unique_ptr<IBlockHasher> hasher = create_hasher(HashAlgorithm::Crc32);
    EXPECT_EQ(hasher->hash(block_from("Hello")), "f7d18982");
}

// 1.2. CRC32 возвращает разные хэши для разных блоков.
TEST(HasherCrc32, DifferentBlocksShouldReturnDifferentHashes) {
    const std::unique_ptr<IBlockHasher> hasher = create_hasher(HashAlgorithm::Crc32);
    EXPECT_NE(hasher->hash(block_from("Hello")), hasher->hash(block_from("World")));
}
#endif

#if (1)  // Группа 2. MD5
// 2.1. MD5 возвращает стабильный хэш для одинаковых блоков.
TEST(HasherMd5, SameBlocksShouldReturnSameHash) {
    const std::unique_ptr<IBlockHasher> hasher = create_hasher(HashAlgorithm::Md5);
    const std::vector<char> block = block_from("Hello");
    EXPECT_EQ(hasher->hash(block), hasher->hash(block));
}

// 2.2. MD5 возвращает разные хэши для разных блоков.
TEST(HasherMd5, DifferentBlocksShouldReturnDifferentHashes) {
    const std::unique_ptr<IBlockHasher> hasher = create_hasher(HashAlgorithm::Md5);
    EXPECT_NE(hasher->hash(block_from("Hello")), hasher->hash(block_from("World")));
}

// 2.3. MD5 возвращает строку из 32 hex-символов.
TEST(HasherMd5, HashShouldHaveExpectedLength) {
    const std::unique_ptr<IBlockHasher> hasher = create_hasher(HashAlgorithm::Md5);
    EXPECT_EQ(hasher->hash(block_from("Hello")).size(), 32U);
}
#endif

#if (1)  // Группа 3. Ошибки выбора алгоритма
// 3.1. Фабрика отклоняет неизвестный алгоритм хэширования.
TEST(HasherFactory, UnknownAlgorithmShouldThrow) {
    EXPECT_THROW(create_hasher(static_cast<HashAlgorithm>(-1)), std::invalid_argument);
}
#endif
