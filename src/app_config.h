#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <boost/filesystem/path.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "hash_algorithm.h"

struct AppConfig {
    std::vector<boost::filesystem::path> include_dirs;
    std::vector<boost::filesystem::path> exclude_dirs;
    std::size_t scan_depth = 0;
    std::uintmax_t min_file_size = 1;
    std::vector<std::string> masks;
    std::size_t block_size = 4096;
    HashAlgorithm hash_algorithm = HashAlgorithm::Crc32;
};

#endif  // APP_CONFIG_H
