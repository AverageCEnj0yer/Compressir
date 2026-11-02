#include "HuffmanCompressor.h"

#include <iostream>

std::vector<std::byte> HuffmanCompressor::compress(
        [[maybe_unused]]const std::vector<std::byte>& data
    ) {
        // TODO: Implement Huffman compression.
        std::cout << "Compressing using Huffman...\n";
        return {};
    }

std::vector<std::byte> HuffmanCompressor::decompress(
        [[maybe_unused]]const std::vector<std::byte>& data
    ) {
        // TODO: Implement Huffman decompression.
        std::cout << "Decompressing using Huffman...\n";
        return {};
    }

CompressionAlgorithm HuffmanCompressor::algorithmType() const {
        return CompressionAlgorithm::Huffman;
    }
