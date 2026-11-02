#ifndef COMPRESSION_REQUEST_H
#define COMPRESSION_REQUEST_H

#include <filesystem>
#include <vector>
#include <cstdint>

enum class CompressionAlgorithm {
    RLE,
    Huffman
};

enum class Operation {
    Compress,
    Decompress
};

struct CompressionRequest {
    Operation operation{Operation::Compress};

    std::vector<std::filesystem::path> files{};
    std::filesystem::path outputPath{};

    CompressionAlgorithm algorithm{CompressionAlgorithm::RLE};
    std::uint8_t compressionLevel{1};

    // TODO: Add archive format selection when needed.
    // TODO: Validate the request before executing it.
};

#endif
