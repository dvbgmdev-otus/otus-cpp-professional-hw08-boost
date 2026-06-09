#include <exception>
#include <iostream>
#include <memory>

#include "cmd_line_parser.h"
#include "duplicate_finder.h"
#include "file_scanner.h"
#include "hasher.h"
#include "output_formatter.h"

int main(int argc, const char* const argv[]) {
    try {
        const CmdLineParser parser;
        const CmdLineParseResult parse_result = parser.parse(argc, argv);

        if (parse_result.help_requested) {
            std::cout << parse_result.help_message;
            return 0;
        }

        const std::unique_ptr<IBlockHasher> hasher =
            create_hasher(parse_result.config.hash_algorithm);

        const FileScanner scanner;
        const std::vector<FileInfo> files = scanner.scan(parse_result.config);

        const DuplicateFinder finder(parse_result.config.block_size, *hasher);
        const std::vector<std::vector<boost::filesystem::path>> duplicate_groups =
            finder.find(files);

        print_duplicate_groups(duplicate_groups, std::cout);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
