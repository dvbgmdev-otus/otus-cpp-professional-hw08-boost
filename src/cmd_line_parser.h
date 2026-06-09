#ifndef CMD_LINE_PARSER_H
#define CMD_LINE_PARSER_H

#include <string>

#include "app_config.h"

/**
 * @brief Результат разбора параметров командной строки.
 * @ingroup cli_group
 */
struct CmdLineParseResult {
    /// Конфигурация приложения.
    AppConfig config;
    /// Признак запроса справки через параметр `--help`.
    bool help_requested = false;
    /// Текст справки по доступным параметрам.
    std::string help_message;
};

/**
 * @brief Парсер параметров командной строки.
 * @ingroup cli_group
 */
class CmdLineParser {
public:
    /**
     * @brief Разбирает аргументы запуска приложения.
     * @param argc Количество аргументов.
     * @param argv Массив аргументов.
     * @return Результат разбора с конфигурацией или признаком запроса справки.
     * @throws std::exception при ошибках разбора или некорректных значениях
     *         параметров.
     */
    CmdLineParseResult parse(int argc, const char* const argv[]) const;
};

#endif  // CMD_LINE_PARSER_H
