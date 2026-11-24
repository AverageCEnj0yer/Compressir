#include "io/FileReader.h"

#include <fstream>
#include <stdexcept>

std::vector<std::byte> FileReader::readData(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("File does not exist: " + path.string());
    }
    if (!std::filesystem::is_regular_file(path)) {
        throw std::runtime_error("Path is not a regular file: " + path.string());
    }

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading: " + path.string());
    }

    const std::ifstream::pos_type endPos = file.tellg();
    if (endPos < 0) {
        throw std::runtime_error("Failed to determine file size: " + path.string());
    }

    const auto size = static_cast<std::size_t>(endPos);
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> buffer(size);
    if (size > 0) {
        if (!file.read(reinterpret_cast<char*>(buffer.data()),
                       static_cast<std::streamsize>(size))) {
            throw std::runtime_error("Failed to read file contents: " + path.string());
        }
    }

    return buffer;
}
