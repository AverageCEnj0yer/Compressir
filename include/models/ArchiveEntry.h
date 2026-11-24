#ifndef ARCHIVE_ENTRY_H
#define ARCHIVE_ENTRY_H

#include "models/CompressionRequest.h"

#include <filesystem>
#include <vector>
#include <cstddef>
#include <cstdint>

struct ArchiveEntry {
    std::filesystem::path relativePath{};

    std::uint64_t originalSize{0};

    CompressionAlgorithm algorithm{CompressionAlgorithm::RLE};
    bool hasChecksum{false};
    bool isEncrypted{false};

    std::vector<std::byte> compressedData{};
};

#endif
