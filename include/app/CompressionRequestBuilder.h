#ifndef COMPRESSION_REQUEST_BUILDER_H
#define COMPRESSION_REQUEST_BUILDER_H

#include "models/CompressionRequest.h"

class CompressionRequestBuilder {
private:
    CompressionRequest m_request{};

public:
    CompressionRequestBuilder& operation(Operation operation);
    CompressionRequestBuilder& addFile(const std::filesystem::path& path);
    CompressionRequestBuilder& output(const std::filesystem::path& path);
    CompressionRequestBuilder& algorithm(CompressionAlgorithm algorithm);
    CompressionRequestBuilder& automaticAlgorithmSelection();
    CompressionRequestBuilder& compressionLevel(std::uint8_t level);
    CompressionRequestBuilder& checksum(bool enabled = true);
    CompressionRequestBuilder& encryption(bool enabled = true, std::uint8_t key = 0x5A);
    CompressionRequest build() const;
};

#endif
