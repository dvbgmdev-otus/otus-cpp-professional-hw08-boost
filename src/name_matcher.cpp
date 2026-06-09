#include "name_matcher.h"

#include <algorithm>
#include <cctype>

namespace {

/**
 * @brief Переводит строку в нижний регистр.
 * @param value Исходная строка.
 * @return Строка в нижнем регистре.
 */
std::string to_lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });

    return value;
}

/**
 * @brief Проверяет соответствие строки маске с wildcard-символами.
 * @param pattern Маска, где `*` означает любую последовательность,
 *        а `?` — один символ.
 * @param value Проверяемая строка.
 * @return `true`, если строка соответствует маске.
 */
bool wildcard_match(const std::string& pattern, const std::string& value) {
    std::size_t pattern_pos = 0;
    std::size_t value_pos = 0;
    std::size_t star_pos = std::string::npos;
    std::size_t star_value_pos = 0;

    while (value_pos < value.size()) {
        if (pattern_pos < pattern.size() &&
            (pattern[pattern_pos] == '?' || pattern[pattern_pos] == value[value_pos])) {
            ++pattern_pos;
            ++value_pos;
        } else if (pattern_pos < pattern.size() && pattern[pattern_pos] == '*') {
            star_pos = pattern_pos;
            star_value_pos = value_pos;
            ++pattern_pos;
        } else if (star_pos != std::string::npos) {
            pattern_pos = star_pos + 1;
            ++star_value_pos;
            value_pos = star_value_pos;
        } else {
            return false;
        }
    }

    while (pattern_pos < pattern.size() && pattern[pattern_pos] == '*') {
        ++pattern_pos;
    }

    return pattern_pos == pattern.size();
}

}  // namespace

NameMatcher::NameMatcher(std::vector<std::string> masks) : m_masks(std::move(masks)) {
    std::transform(m_masks.begin(), m_masks.end(), m_masks.begin(), [](const std::string& mask) {
        return to_lower(mask);
    });
}

bool NameMatcher::matches(const boost::filesystem::path& path) const {
    if (m_masks.empty()) {
        return true;
    }

    const std::string filename = to_lower(path.filename().string());

    return std::any_of(m_masks.begin(), m_masks.end(), [&filename](const std::string& mask) {
        return wildcard_match(mask, filename);
    });
}
