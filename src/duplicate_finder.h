#ifndef DUPLICATE_FINDER_H
#define DUPLICATE_FINDER_H

#include <boost/filesystem/path.hpp>
#include <cstddef>
#include <vector>

#include "file_info.h"
#include "hasher.h"

/**
 * @brief Ищет группы файлов с одинаковым содержимым.
 * @ingroup finder_group
 */
class DuplicateFinder {
public:
    /**
     * @brief Создаёт объект поиска дубликатов.
     * @param block_size Размер блока сравнения.
     * @param hasher Хэшер блоков.
     * @throws std::invalid_argument если размер блока равен нулю.
     */
    DuplicateFinder(std::size_t block_size, const IBlockHasher& hasher);

    /**
     * @brief Находит дубликаты среди переданных файлов.
     * @param files Файлы-кандидаты.
     * @return Группы путей к файлам с одинаковым содержимым.
     */
    std::vector<std::vector<boost::filesystem::path>> find(
        const std::vector<FileInfo>& files) const;

private:
    /// Размер блока сравнения.
    std::size_t m_block_size;
    /// Хэшер блоков.
    const IBlockHasher& m_hasher;
};

#endif  // DUPLICATE_FINDER_H
