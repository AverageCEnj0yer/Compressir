#include "FileWriter.h"

#include <iostream>

bool FileWriter::writeData(
        const std::filesystem::path& path,
        [[maybe_unused]] const std::vector<std::byte>& data
    ) {
        // TODO: Open/create output file in binary mode.
        // TODO: Write provided bytes.
        // TODO: Handle writing errors.

        std::cout << "Writing: " << path << '\n';

        return true; // Placeholder only.
    }
