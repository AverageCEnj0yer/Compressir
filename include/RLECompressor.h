#ifndef RLE_COMPRESSOR_H
#define RLE_COMPRESSOR_H

#include "ICompressionAlgorithm.h"
#include <iostream>

class RLECompressor : public ICompressionAlgorithm {
public:
    std::vector<std::byte> compress(
        [[maybe_unused]]const std::vector<std::byte>& data
    ) override {
        // TODO: Implement RLE compression.
        std::cout << "Compressing using RLE...\n";
        return {};
    }

    std::vector<std::byte> decompress(
        [[maybe_unused]]const std::vector<std::byte>& data
    ) override {
        // TODO: Implement RLE decompression.
        std::cout << "Decompressing using RLE...\n";
        return {};
    }

    CompressionAlgorithm algorithmType() const override {
        return CompressionAlgorithm::RLE;
    }
};

#endif
