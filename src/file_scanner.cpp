#include "file_scanner.h"

#include <algorithm>
#include <boost/filesystem.hpp>

#include "name_matcher.h"

namespace {

namespace fs = boost::filesystem;

fs::path normalized_path(const fs::path& path) {
    boost::system::error_code error;
    const fs::path canonical_path = fs::canonical(path, error);
    if (!error) {
        return canonical_path;
    }
    return fs::absolute(path);
}

bool is_excluded_directory(const fs::path& path, const std::vector<fs::path>& exclude_dirs) {
    const fs::path current = normalized_path(path);
    return std::any_of(
        exclude_dirs.begin(), exclude_dirs.end(), [&current](const fs::path& exclude_dir) {
            return current == normalized_path(exclude_dir);
        });
}

bool path_is_regular_file(const fs::path& path) {
    boost::system::error_code error;
    return fs::is_regular_file(path, error) && !error;
}

bool path_is_directory(const fs::path& path) {
    boost::system::error_code error;
    return fs::is_directory(path, error) && !error;
}

std::uintmax_t regular_file_size(const fs::path& path) {
    boost::system::error_code error;
    const std::uintmax_t size = fs::file_size(path, error);
    if (error) {
        return 0;  // LCOV_EXCL_LINE
    }
    return size;
}

}  // namespace

std::vector<FileInfo> FileScanner::scan(const AppConfig& config) const {
    std::vector<FileInfo> files;
    const NameMatcher name_matcher(config.masks);

    for (const fs::path& include_dir : config.include_dirs) {
        if (!path_is_directory(include_dir)) {
            continue;
        }

        fs::recursive_directory_iterator it(include_dir);
        const fs::recursive_directory_iterator end;

        while (it != end) {
            const fs::path current_path = it->path();

            if (path_is_directory(current_path)) {
                if (is_excluded_directory(current_path, config.exclude_dirs)) {
                    it.disable_recursion_pending();
                } else if (static_cast<std::size_t>(it.depth() + 1) > config.scan_depth) {
                    it.disable_recursion_pending();
                }

                ++it;
                continue;
            }

            if (path_is_regular_file(current_path)) {
                const std::uintmax_t size = regular_file_size(current_path);
                if (size >= config.min_file_size && name_matcher.matches(current_path)) {
                    files.push_back({ normalized_path(current_path), size });
                }
            }

            ++it;
        }
    }

    return files;
}
