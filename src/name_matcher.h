#ifndef NAME_MATCHER_H
#define NAME_MATCHER_H

#include <boost/filesystem/path.hpp>
#include <string>
#include <vector>

/**
 * @brief Проверяет имя файла на соответствие маскам.
 * @ingroup scan_group
 */
class NameMatcher {
public:
    /**
     * @brief Создаёт проверяющий объект с заданным набором масок.
     * @param masks Маски имён файлов.
     */
    explicit NameMatcher(std::vector<std::string> masks);

    /**
     * @brief Проверяет путь на соответствие хотя бы одной маске.
     * @param path Проверяемый путь.
     * @return `true`, если маски не заданы или имя файла подходит под маску.
     */
    bool matches(const boost::filesystem::path& path) const;

private:
    /// Маски имён файлов.
    std::vector<std::string> m_masks;
};

#endif  // NAME_MATCHER_H
