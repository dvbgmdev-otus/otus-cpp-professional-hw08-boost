#ifndef DUPLICATE_FINDER_H
#define DUPLICATE_FINDER_H

#include "file_info.h"
#include "hasher.h"

#include <boost/filesystem/path.hpp>

#include <cstddef>
#include <vector>

class DuplicateFinder {
public:
    DuplicateFinder(std::size_t block_size, const IBlockHasher& hasher);

    std::vector<std::vector<boost::filesystem::path>> find(const std::vector<FileInfo>& files) const;

private:
    std::size_t m_block_size;
    const IBlockHasher& m_hasher;
};

#endif  // DUPLICATE_FINDER_H
