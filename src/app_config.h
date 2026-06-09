#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <boost/filesystem/path.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "hash_algorithm.h"

/**
 * @brief Настройки запуска утилиты.
 * @ingroup config_group
 *
 * Структура формируется парсером командной строки и передаётся остальным
 * компонентам приложения как единый набор параметров.
 */
struct AppConfig {
    /// Директории, в которых выполняется поиск файлов.
    std::vector<boost::filesystem::path> include_dirs;
    /// Директории, исключённые из обхода.
    std::vector<boost::filesystem::path> exclude_dirs;
    /// Глубина рекурсивного обхода директорий.
    std::size_t scan_depth = 0;
    /// Минимальный размер файла для участия в поиске дубликатов.
    std::uintmax_t min_file_size = 1;
    /// Маски имён файлов, ограничивающие набор кандидатов.
    std::vector<std::string> masks;
    /// Размер блока, используемый при чтении и хэшировании файлов.
    std::size_t block_size = 4096;
    /// Алгоритм хэширования блоков.
    HashAlgorithm hash_algorithm = HashAlgorithm::Crc32;
};

#endif  // APP_CONFIG_H
