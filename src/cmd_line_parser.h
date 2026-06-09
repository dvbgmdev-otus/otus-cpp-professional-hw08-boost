#ifndef CMD_LINE_PARSER_H
#define CMD_LINE_PARSER_H

#include <string>

#include "app_config.h"

struct CmdLineParseResult {
    AppConfig config;
    bool help_requested = false;
    std::string help_message;
};

class CmdLineParser {
public:
    CmdLineParseResult parse(int argc, const char* const argv[]) const;
};

#endif  // CMD_LINE_PARSER_H
