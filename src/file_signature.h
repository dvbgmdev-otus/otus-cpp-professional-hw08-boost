#ifndef FILE_SIGNATURE_H
#define FILE_SIGNATURE_H

#include <boost/optional.hpp>
#include <cstddef>
#include <string>
#include <vector>

#include "file_info.h"
#include "hasher.h"

/**
 * @brief Лениво вычисляемая сигнатура файла.
 * @ingroup signature_group
 *
 * Сигнатура представляет файл последовательностью хэшей блоков и кэширует
 * уже рассчитанные значения.
 */
class FileSignature {
public:
    /**
     * @brief Создаёт сигнатуру для файла.
     * @param file Информация о файле.
     * @param block_size Размер блока чтения.
     * @param hasher Хэшер блоков.
     * @throws std::invalid_argument если размер блока равен нулю.
     */
    FileSignature(FileInfo file, std::size_t block_size, const IBlockHasher& hasher);

    /**
     * @brief Возвращает путь к файлу.
     * @return Путь к файлу.
     */
    const boost::filesystem::path& path() const;

    /**
     * @brief Возвращает размер файла.
     * @return Размер файла в байтах.
     */
    std::uintmax_t size() const;

    /**
     * @brief Возвращает количество блоков файла.
     * @return Количество блоков, участвующих в сигнатуре.
     */
    std::size_t block_count() const;

    /**
     * @brief Возвращает хэш блока с заданным индексом.
     * @param block_index Индекс блока.
     * @return Строковое представление хэша блока.
     * @throws std::out_of_range если индекс блока выходит за границы файла.
     * @throws std::runtime_error если файл не удалось открыть для чтения.
     */
    std::string block_hash(std::size_t block_index);

private:
    /// Информация о файле.
    FileInfo m_file;
    /// Размер блока чтения.
    std::size_t m_block_size;
    /// Хэшер блоков.
    const IBlockHasher& m_hasher;
    /// Кэш рассчитанных хэшей блоков.
    std::vector<boost::optional<std::string>> m_block_hashes;
};

#endif  // FILE_SIGNATURE_H
