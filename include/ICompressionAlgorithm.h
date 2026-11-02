#ifndef ICOMPRESSIONALGORITHM_H
#define ICOMPRESSIONALGORITHM_H

#include "CompressionRequest.h"

#include <vector>
#include <cstddef>

class ICompressionAlgorithm {
public:
    virtual ~ICompressionAlgorithm() = default;

    virtual std::vector<std::byte> compress(
        const std::vector<std::byte>& data
    ) = 0;

    virtual std::vector<std::byte> decompress(
        const std::vector<std::byte>& data
    ) = 0;

    virtual CompressionAlgorithm algorithmType() const = 0;
};

#endif
