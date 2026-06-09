#ifndef FILE_SCANNER_H
#define FILE_SCANNER_H

#include <vector>

#include "app_config.h"
#include "file_info.h"

/**
 * @brief Сканирует файловую систему и выбирает файлы-кандидаты.
 * @ingroup scan_group
 */
class FileScanner {
public:
    /**
     * @brief Выполняет обход директорий согласно конфигурации.
     * @param config Настройки сканирования.
     * @return Список найденных файлов, прошедших первичную фильтрацию.
     */
    std::vector<FileInfo> scan(const AppConfig& config) const;
};

#endif  // FILE_SCANNER_H
