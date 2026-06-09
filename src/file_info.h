#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <boost/filesystem/path.hpp>
#include <cstdint>

struct FileInfo {
    boost::filesystem::path path;
    std::uintmax_t size = 0;
};

#endif  // FILE_INFO_H
