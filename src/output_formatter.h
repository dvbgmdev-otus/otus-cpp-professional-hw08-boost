#ifndef OUTPUT_FORMATTER_H
#define OUTPUT_FORMATTER_H

#include <boost/filesystem/path.hpp>
#include <iosfwd>
#include <vector>

/**
 * @brief Выводит группы дубликатов в поток.
 * @ingroup output_group
 * @param groups Группы путей к файлам-дубликатам.
 * @param out Поток вывода.
 */
void print_duplicate_groups(const std::vector<std::vector<boost::filesystem::path>>& groups,
                            std::ostream& out);

#endif  // OUTPUT_FORMATTER_H
