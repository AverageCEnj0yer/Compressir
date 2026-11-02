#ifndef HUFFMAN_COMPRESSOR_H
#define HUFFMAN_COMPRESSOR_H

#include "ICompressionAlgorithm.h"
#include <iostream>

class HuffmanCompressor : public ICompressionAlgorithm {
public:
    std::vector<std::byte> compress(
        [[maybe_unused]]const std::vector<std::byte>& data
    ) override {
        // TODO: Implement Huffman compression.
        std::cout << "Compressing using Huffman...\n";
        return {};
    }

    std::vector<std::byte> decompress(
        [[maybe_unused]]const std::vector<std::byte>& data
    ) override {
        // TODO: Implement Huffman decompression.
        std::cout << "Decompressing using Huffman...\n";
        return {};
    }

    CompressionAlgorithm algorithmType() const override {
        return CompressionAlgorithm::Huffman;
    }
};

#endif
