#ifndef FILE_READER_H
#define FILE_READER_H

#include "IReader.h"
#include <iostream>

class FileReader : public IReader {
public:
    std::vector<std::byte> readData(
        const std::filesystem::path& path
    ) override {
        // TODO: Open file in binary mode.
        // TODO: Read and return its bytes.
        // TODO: Handle file reading errors.

        std::cout << "Reading: " << path << '\n';

        return {};
    }
};

#endif
