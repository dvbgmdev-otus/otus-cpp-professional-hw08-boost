#ifndef NAME_MATCHER_H
#define NAME_MATCHER_H

#include <boost/filesystem/path.hpp>
#include <string>
#include <vector>

class NameMatcher {
public:
    explicit NameMatcher(std::vector<std::string> masks);

    bool matches(const boost::filesystem::path& path) const;

private:
    std::vector<std::string> m_masks;
};

#endif  // NAME_MATCHER_H
