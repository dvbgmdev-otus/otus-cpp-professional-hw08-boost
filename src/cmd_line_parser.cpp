#include "cmd_line_parser.h"

#include <boost/program_options.hpp>
#include <sstream>
#include <stdexcept>

namespace {

namespace po = boost::program_options;

HashAlgorithm parse_hash_algorithm(const std::string& value) {
    if (value == "crc32") {
        return HashAlgorithm::Crc32;
    }
    if (value == "md5") {
        return HashAlgorithm::Md5;
    }
    throw std::invalid_argument("Unknown hash algorithm: " + value);
}

std::vector<boost::filesystem::path> to_paths(const std::vector<std::string>& values) {
    std::vector<boost::filesystem::path> paths;
    paths.reserve(values.size());
    for (const std::string& value : values) {
        paths.push_back(value);
    }
    return paths;
} // LCOV_EXCL_LINE

std::string make_help_message(const po::options_description& description) {
    std::ostringstream stream;
    stream << description;
    return stream.str();
}

}  // namespace

CmdLineParseResult CmdLineParser::parse(int argc, const char* const argv[]) const {
    po::options_description description("Allowed options");
    description.add_options()("help", "show help message")(
        "include", po::value<std::vector<std::string>>()->multitoken(), "directory to scan")(
        "exclude", po::value<std::vector<std::string>>()->multitoken(), "directory to exclude")(
        "level", po::value<std::size_t>()->default_value(0), "scan depth")(
        "min-size", po::value<std::uintmax_t>()->default_value(1), "minimal file size")(
        "mask", po::value<std::vector<std::string>>()->multitoken(), "file name mask")(
        "block-size", po::value<std::size_t>()->default_value(4096), "read block size")(
        "hash", po::value<std::string>()->default_value("crc32"), "hash algorithm: crc32 or md5");

    po::variables_map variables;
    po::store(po::parse_command_line(argc, argv, description), variables);
    po::notify(variables);

    CmdLineParseResult result;
    result.help_message = make_help_message(description);

    if (variables.count("help") != 0) {
        result.help_requested = true;
        return result;
    }

    if (variables.count("include") != 0) {
        result.config.include_dirs = to_paths(variables["include"].as<std::vector<std::string>>());
    } else {
        result.config.include_dirs = { "." };
    }

    if (variables.count("exclude") != 0) {
        result.config.exclude_dirs = to_paths(variables["exclude"].as<std::vector<std::string>>());
    }

    if (variables.count("mask") != 0) {
        result.config.masks = variables["mask"].as<std::vector<std::string>>();
    }

    result.config.scan_depth = variables["level"].as<std::size_t>();
    result.config.min_file_size = variables["min-size"].as<std::uintmax_t>();
    result.config.block_size = variables["block-size"].as<std::size_t>();
    if (result.config.block_size == 0) {
        throw std::invalid_argument("Block size must be greater than zero");
    }

    result.config.hash_algorithm = parse_hash_algorithm(variables["hash"].as<std::string>());

    return result;
}
