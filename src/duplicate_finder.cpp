#include "duplicate_finder.h"

#include <map>
#include <stdexcept>
#include <string>

#include "file_signature.h"

namespace {

using SignatureIndexGroup = std::vector<std::size_t>;

/**
 * @brief Оставляет только группы индексов, в которых есть дубликаты.
 * @param groups Группы индексов сигнатур, сгруппированные по хэшу.
 * @return Группы, содержащие больше одного индекса.
 */
std::vector<SignatureIndexGroup> keep_groups_with_duplicates(
    const std::map<std::string, SignatureIndexGroup>& groups) {
    std::vector<SignatureIndexGroup> result;
    for (const auto& group : groups) {
        if (group.second.size() > 1) {
            result.push_back(group.second);
        }
    }
    return result;
}  // LCOV_EXCL_LINE

}  // namespace

DuplicateFinder::DuplicateFinder(std::size_t block_size, const IBlockHasher& hasher)
    : m_block_size(block_size), m_hasher(hasher) {
    if (m_block_size == 0) {
        throw std::invalid_argument("Block size must be greater than zero");
    }
}

std::vector<std::vector<boost::filesystem::path>> DuplicateFinder::find(
    const std::vector<FileInfo>& files) const {
    std::map<std::uintmax_t, std::vector<FileInfo>> files_by_size;
    for (const FileInfo& file : files) {
        files_by_size[file.size].push_back(file);
    }

    std::vector<std::vector<boost::filesystem::path>> duplicates;

    for (const auto& size_group : files_by_size) {
        if (size_group.second.size() < 2) {
            continue;
        }

        std::vector<FileSignature> signatures;
        signatures.reserve(size_group.second.size());
        for (const FileInfo& file : size_group.second) {
            signatures.emplace_back(file, m_block_size, m_hasher);
        }

        std::vector<SignatureIndexGroup> candidate_groups(1);
        for (std::size_t index = 0; index < signatures.size(); ++index) {
            candidate_groups.front().push_back(index);
        }

        const std::size_t block_count = signatures.front().block_count();
        for (std::size_t block_index = 0; block_index < block_count; ++block_index) {
            std::vector<SignatureIndexGroup> next_candidate_groups;

            for (const SignatureIndexGroup& candidate_group : candidate_groups) {
                std::map<std::string, SignatureIndexGroup> groups_by_hash;
                for (const std::size_t signature_index : candidate_group) {
                    groups_by_hash[signatures[signature_index].block_hash(block_index)].push_back(
                        signature_index);
                }

                std::vector<SignatureIndexGroup> groups_with_duplicates =
                    keep_groups_with_duplicates(groups_by_hash);
                next_candidate_groups.insert(next_candidate_groups.end(),
                                             groups_with_duplicates.begin(),
                                             groups_with_duplicates.end());
            }

            candidate_groups = std::move(next_candidate_groups);
            if (candidate_groups.empty()) {
                break;
            }
        }

        for (const SignatureIndexGroup& candidate_group : candidate_groups) {
            std::vector<boost::filesystem::path> duplicate_group;
            for (const std::size_t signature_index : candidate_group) {
                duplicate_group.push_back(signatures[signature_index].path());
            }
            duplicates.push_back(duplicate_group);
        }
    }

    return duplicates;
}
