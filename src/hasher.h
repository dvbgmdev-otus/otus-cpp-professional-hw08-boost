#ifndef HASHER_H
#define HASHER_H

#include <memory>
#include <string>
#include <vector>

#include "hash_algorithm.h"

class IBlockHasher {
public:
    virtual ~IBlockHasher() = default;

    virtual std::string hash(const std::vector<char>& block) const = 0;
};

std::unique_ptr<IBlockHasher> create_hasher(HashAlgorithm algorithm);

#endif  // HASHER_H
