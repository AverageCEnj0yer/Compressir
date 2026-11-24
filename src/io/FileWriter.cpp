#include "io/FileWriter.h"

#include <fstream>
#include <system_error>

bool FileWriter::writeData(const std::filesystem::path& path,
                           const std::vector<std::byte>& data) {
    if (path.empty()) {
        return false;
    }

    if (path.has_parent_path() && !path.parent_path().empty()) {
        std::error_code ec;
        std::filesystem::create_directories(path.parent_path(), ec);
        if (ec) {
            return false;
        }
    }

    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) {
        return false;
    }

    if (!data.empty()) {
        file.write(reinterpret_cast<const char*>(data.data()),
                   static_cast<std::streamsize>(data.size()));
    }

    return static_cast<bool>(file);
}
