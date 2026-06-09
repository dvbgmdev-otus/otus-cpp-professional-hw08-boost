#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <boost/filesystem/path.hpp>
#include <cstdint>

/**
 * @brief Информация о файле-кандидате.
 * @ingroup config_group
 */
struct FileInfo {
    /// Путь к файлу.
    boost::filesystem::path path;
    /// Размер файла в байтах.
    std::uintmax_t size = 0;
};

#endif  // FILE_INFO_H
