#ifndef HUFFMAN_COMPRESSOR_H
#define HUFFMAN_COMPRESSOR_H

#include "ICompressionAlgorithm.h"

class HuffmanCompressor : public ICompressionAlgorithm {
public:
    std::vector<std::byte> compress(
        const std::vector<std::byte>& data
    ) override;

    std::vector<std::byte> decompress(
        const std::vector<std::byte>& data
    ) override;

    CompressionAlgorithm algorithmType() const override;
};

#endif
