#include "RLECompressor.h"

#include <iostream>

std::vector<std::byte> RLECompressor::compress(
        [[maybe_unused]]const std::vector<std::byte>& data
    ) {
        // TODO: Implement RLE compression.
        std::cout << "Compressing using RLE...\n";
        return {};
    }

std::vector<std::byte> RLECompressor::decompress(
        [[maybe_unused]]const std::vector<std::byte>& data
    ) {
        // TODO: Implement RLE decompression.
        std::cout << "Decompressing using RLE...\n";
        return {};
    }

CompressionAlgorithm RLECompressor::algorithmType() const {
        return CompressionAlgorithm::RLE;
    }
