#include "AlgorithmCreator.h"
#include "RLECompressor.h"
#include "HuffmanCompressor.h"
#include <stdexcept>

std::unique_ptr<ICompressionAlgorithm> RLECreator::createAlgorithm() const {
    return std::make_unique<RLECompressor>();
}

std::unique_ptr<ICompressionAlgorithm> HuffmanCreator::createAlgorithm() const {
    return std::make_unique<HuffmanCompressor>();
}

std::unique_ptr<AlgorithmCreator> createAlgorithmCreator(CompressionAlgorithm algorithm) {
    switch (algorithm) {
        case CompressionAlgorithm::RLE:
            return std::make_unique<RLECreator>();
        case CompressionAlgorithm::Huffman:
            return std::make_unique<HuffmanCreator>();
    }
    throw std::invalid_argument("Unknown compression algorithm");
}
