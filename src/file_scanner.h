#ifndef FILE_SCANNER_H
#define FILE_SCANNER_H

#include <vector>

#include "app_config.h"
#include "file_info.h"

class FileScanner {
public:
    std::vector<FileInfo> scan(const AppConfig& config) const;
};

#endif  // FILE_SCANNER_H
