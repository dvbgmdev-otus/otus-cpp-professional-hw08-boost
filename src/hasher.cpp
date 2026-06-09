#include "hasher.h"

#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

std::string to_hex(std::uint32_t value) {
    std::ostringstream stream;
    stream << std::hex << std::setfill('0') << std::setw(8) << value;
    return stream.str();
}

class Crc32Hasher final : public IBlockHasher {
public:
    std::string hash(const std::vector<char>& block) const override {
        boost::crc_32_type crc;
        crc.process_bytes(block.data(), block.size());
        return to_hex(crc.checksum());
    }
};

class Md5Hasher final : public IBlockHasher {
public:
    std::string hash(const std::vector<char>& block) const override {
        boost::uuids::detail::md5 md5;
        md5.process_bytes(block.data(), block.size());

        boost::uuids::detail::md5::digest_type digest;
        md5.get_digest(digest);

        std::ostringstream stream;
        stream << std::hex << std::setfill('0');
        for (const auto value : digest) {
            stream << std::setw(sizeof(value) * 2) << static_cast<std::uint64_t>(value);
        }

        return stream.str();
    }
};

}  // namespace

std::unique_ptr<IBlockHasher> create_hasher(HashAlgorithm algorithm) {
    switch (algorithm) {
        case HashAlgorithm::Crc32:
            return std::unique_ptr<IBlockHasher>(new Crc32Hasher());
        case HashAlgorithm::Md5:
            return std::unique_ptr<IBlockHasher>(new Md5Hasher());
    }

    throw std::invalid_argument("Unknown hash algorithm");
}
