#ifndef FILE_SIGNATURE_H
#define FILE_SIGNATURE_H

#include <boost/optional.hpp>
#include <cstddef>
#include <string>
#include <vector>

#include "file_info.h"
#include "hasher.h"

class FileSignature {
public:
    FileSignature(FileInfo file, std::size_t block_size, const IBlockHasher& hasher);

    const boost::filesystem::path& path() const;
    std::uintmax_t size() const;
    std::size_t block_count() const;

    std::string block_hash(std::size_t block_index);

private:
    FileInfo m_file;
    std::size_t m_block_size;
    const IBlockHasher& m_hasher;
    std::vector<boost::optional<std::string>> m_block_hashes;
};

#endif  // FILE_SIGNATURE_H
