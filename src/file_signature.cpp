#include "file_signature.h"

#include <fstream>
#include <stdexcept>

namespace {

/**
 * @brief Рассчитывает количество блоков для файла.
 * @param file_size Размер файла в байтах.
 * @param block_size Размер блока чтения.
 * @return Количество блоков с округлением вверх.
 */
std::size_t calculate_block_count(std::uintmax_t file_size, std::size_t block_size) {
    if (file_size == 0) {
        return 0;
    }
    return static_cast<std::size_t>((file_size + block_size - 1) / block_size);
}

}  // namespace

FileSignature::FileSignature(FileInfo file, std::size_t block_size, const IBlockHasher& hasher)
    : m_file(std::move(file)), m_block_size(block_size), m_hasher(hasher) {
    if (m_block_size == 0) {
        throw std::invalid_argument("Block size must be greater than zero");
    }

    m_block_hashes.resize(calculate_block_count(m_file.size, m_block_size));
}

const boost::filesystem::path& FileSignature::path() const { return m_file.path; }

std::uintmax_t FileSignature::size() const { return m_file.size; }

std::size_t FileSignature::block_count() const { return m_block_hashes.size(); }

std::string FileSignature::block_hash(std::size_t block_index) {
    if (block_index >= block_count()) {
        throw std::out_of_range("Block index is out of range");
    }

    if (m_block_hashes[block_index]) {
        return *m_block_hashes[block_index];
    }

    std::vector<char> block(m_block_size, '\0');
    std::ifstream file(path().string(), std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to open file");
    }

    file.seekg(static_cast<std::streamoff>(block_index * m_block_size));
    file.read(block.data(), static_cast<std::streamsize>(block.size()));

    const std::string hash = m_hasher.hash(block);
    m_block_hashes[block_index] = hash;

    return hash;
}
