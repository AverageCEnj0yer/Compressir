#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include "IWriter.h"
#include <iostream>

class FileWriter : public IWriter {
public:
    bool writeData(
        const std::filesystem::path& path,
        [[maybe_unused]] const std::vector<std::byte>& data
    ) override {
        // TODO: Open/create output file in binary mode.
        // TODO: Write provided bytes.
        // TODO: Handle writing errors.

        std::cout << "Writing: " << path << '\n';

        return true; // Placeholder only.
    }
};

#endif
