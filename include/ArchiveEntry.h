#ifndef ARCHIVE_ENTRY_H
#define ARCHIVE_ENTRY_H

#include "CompressionRequest.h"

#include <filesystem>
#include <vector>
#include <cstddef>
#include <cstdint>

struct ArchiveEntry {
    std::filesystem::path relativePath{};

    std::uint64_t originalSize{0};

    CompressionAlgorithm algorithm{CompressionAlgorithm::RLE};

    std::vector<std::byte> compressedData{};

    // TODO: Add any additional metadata needed
    // for storing and restoring individual files.
};

#endif
