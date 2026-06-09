#include "output_formatter.h"

#include <ostream>

void print_duplicate_groups(const std::vector<std::vector<boost::filesystem::path>>& groups,
                            std::ostream& out) {
    for (std::size_t group_index = 0; group_index < groups.size(); ++group_index) {
        if (group_index > 0) {
            out << '\n';
        }

        for (const boost::filesystem::path& path : groups[group_index]) {
            out << path.string() << '\n';
        }
    }
}
