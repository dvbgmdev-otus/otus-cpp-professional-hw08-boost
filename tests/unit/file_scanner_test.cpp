/**
 * @file file_scanner_test.cpp
 * @brief Тесты сканирования файлов по настройкам приложения.
 */

#include "file_scanner.h"

#include <boost/filesystem.hpp>

#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {

namespace fs = boost::filesystem;

class FileScannerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_root = fs::temp_directory_path() / fs::unique_path("bayan_file_scanner_%%%%-%%%%-%%%%");
        fs::create_directories(m_root);
    }

    void TearDown() override {
        boost::system::error_code error;
        fs::remove_all(m_root, error);
    }

    fs::path make_dir(const std::string& relative_path) const {
        const fs::path path = m_root / relative_path;
        fs::create_directories(path);
        return path;
    }

    fs::path make_file(const std::string& relative_path, const std::string& content) const {
        const fs::path path = m_root / relative_path;
        fs::create_directories(path.parent_path());

        std::ofstream file(path.string(), std::ios::binary);
        file << content;

        return path;
    }

    static std::vector<fs::path> paths_from(const std::vector<FileInfo>& files) {
        std::vector<fs::path> paths;
        for (const FileInfo& file : files) {
            paths.push_back(file.path);
        }

        return paths;
    }

    fs::path m_root;
    FileScanner m_scanner;
};

}  // namespace

#if (1)  // Группа 1. Базовая фильтрация файлов
// 1.1. Сканер возвращает файлы из указанной директории.
TEST_F(FileScannerTest, IncludeDirectoryShouldReturnRegularFiles) {
    const fs::path file = make_file("file.txt", "content");
    AppConfig config;
    config.include_dirs = { m_root };
    const std::vector<FileInfo> files = m_scanner.scan(config);
    EXPECT_EQ(paths_from(files), std::vector<fs::path>({ fs::canonical(file) }));
}

// 1.2. Сканер игнорирует файлы меньше минимального размера.
TEST_F(FileScannerTest, MinFileSizeShouldSkipSmallFiles) {
    make_file("small.txt", "a");
    const fs::path large_file = make_file("large.txt", "abcd");
    AppConfig config;
    config.include_dirs = { m_root };
    config.min_file_size = 2;
    const std::vector<FileInfo> files = m_scanner.scan(config);
    EXPECT_EQ(paths_from(files), std::vector<fs::path>({ fs::canonical(large_file) }));
}

// 1.3. Сканер игнорирует несуществующие директории для сканирования.
TEST_F(FileScannerTest, MissingIncludeDirectoryShouldBeSkipped) {
    AppConfig config;
    config.include_dirs = { m_root / "missing" };
    const std::vector<FileInfo> files = m_scanner.scan(config);
    EXPECT_TRUE(files.empty());
}
#endif

#if (1)  // Группа 2. Фильтрация по маскам
// 2.1. Сканер применяет маски имён файлов без учёта регистра.
TEST_F(FileScannerTest, MasksShouldBeCaseInsensitive) {
    const fs::path text_file = make_file("REPORT.TXT", "content");
    make_file("image.png", "content");
    AppConfig config;
    config.include_dirs = { m_root };
    config.masks = { "*.txt" };
    const std::vector<FileInfo> files = m_scanner.scan(config);
    EXPECT_EQ(paths_from(files), std::vector<fs::path>({ fs::canonical(text_file) }));
}
#endif

#if (1)  // Группа 3. Глубина сканирования
// 3.1. Глубина 0 ограничивает сканирование указанной директорией.
TEST_F(FileScannerTest, ZeroDepthShouldSkipNestedDirectories) {
    const fs::path root_file = make_file("root.txt", "content");
    make_file("nested/file.txt", "content");
    AppConfig config;
    config.include_dirs = { m_root };
    config.scan_depth = 0;
    const std::vector<FileInfo> files = m_scanner.scan(config);
    EXPECT_EQ(paths_from(files), std::vector<fs::path>({ fs::canonical(root_file) }));
}

// 3.2. Глубина 1 разрешает сканирование на один уровень вложенности.
TEST_F(FileScannerTest, DepthOneShouldIncludeDirectNestedFiles) {
    make_file("root.txt", "content");
    const fs::path nested_file = make_file("nested/file.txt", "content");
    make_file("nested/deeper/file.txt", "content");
    AppConfig config;
    config.include_dirs = { m_root };
    config.scan_depth = 1;
    config.masks = { "file.txt" };
    const std::vector<FileInfo> files = m_scanner.scan(config);
    EXPECT_EQ(paths_from(files), std::vector<fs::path>({ fs::canonical(nested_file) }));
}
#endif

#if (1)  // Группа 4. Исключённые директории
// 4.1. Сканер не возвращает файлы из исключённой директории.
TEST_F(FileScannerTest, ExcludedDirectoryShouldBeSkipped) {
    const fs::path included_file = make_file("included/file.txt", "content");
    const fs::path excluded_dir = make_dir("excluded");
    make_file("excluded/file.txt", "content");
    AppConfig config;
    config.include_dirs = { m_root };
    config.exclude_dirs = { excluded_dir };
    config.scan_depth = 1;
    const std::vector<FileInfo> files = m_scanner.scan(config);
    EXPECT_EQ(paths_from(files), std::vector<fs::path>({ fs::canonical(included_file) }));
}

// 4.2. Несуществующая директория исключения не влияет на сканирование.
TEST_F(FileScannerTest, MissingExcludedDirectoryShouldNotAffectScan) {
    const fs::path file = make_file("nested/file.txt", "content");
    AppConfig config;
    config.include_dirs = { m_root };
    config.exclude_dirs = { m_root / "missing" };
    config.scan_depth = 1;
    const std::vector<FileInfo> files = m_scanner.scan(config);
    EXPECT_EQ(paths_from(files), std::vector<fs::path>({ fs::canonical(file) }));
}

// 4.3. Директория сканирования, указанная в исключениях, пропускается целиком.
TEST_F(FileScannerTest, ExcludedIncludeDirectoryShouldBeSkipped) {
    make_file("file.txt", "content");
    AppConfig config;
    config.include_dirs = { m_root };
    config.exclude_dirs = { m_root };
    const std::vector<FileInfo> files = m_scanner.scan(config);
    EXPECT_TRUE(files.empty());
}
#endif
