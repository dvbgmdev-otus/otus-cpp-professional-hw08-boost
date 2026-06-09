#ifndef HASHER_H
#define HASHER_H

#include <memory>
#include <string>
#include <vector>

#include "hash_algorithm.h"

/**
 * @brief Интерфейс хэширования одного блока данных.
 * @ingroup hash_group
 */
class IBlockHasher {
public:
    virtual ~IBlockHasher() = default;

    /**
     * @brief Рассчитывает хэш блока.
     * @param block Данные блока.
     * @return Строковое представление хэша.
     */
    virtual std::string hash(const std::vector<char>& block) const = 0;
};

/**
 * @brief Создаёт реализацию хэшера для выбранного алгоритма.
 * @ingroup hash_group
 * @param algorithm Алгоритм хэширования.
 * @return Объект хэшера.
 */
std::unique_ptr<IBlockHasher> create_hasher(HashAlgorithm algorithm);

#endif  // HASHER_H
