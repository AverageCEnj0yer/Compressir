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
    bool automaticAlgorithmSelection{true};
    std::uint8_t compressionLevel{1};
    bool enableChecksum{false};
    bool enableEncryption{false};
    std::uint8_t encryptionKey{0x5A};
};

#endif
