#ifndef RLE_COMPRESSOR_H
#define RLE_COMPRESSOR_H

#include "ICompressionAlgorithm.h"

class RLECompressor : public ICompressionAlgorithm {
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
