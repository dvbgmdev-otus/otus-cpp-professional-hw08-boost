#ifndef HASH_ALGORITHM_H
#define HASH_ALGORITHM_H

/**
 * @brief Поддерживаемые алгоритмы хэширования блоков.
 * @ingroup config_group
 */
enum class HashAlgorithm {
    /// CRC32.
    Crc32,
    /// MD5.
    Md5
};

#endif  // HASH_ALGORITHM_H
