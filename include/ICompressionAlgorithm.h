#ifndef ICOMPRESSIONALGORITHM_H
#define ICOMPRESSIONALGORITHM_H

#include <cstddef>
#include <vector>

class ICompressionAlgorithm
{
    public:
    virtual std::vector<std::byte> compress(std::vector<std::byte> data) = 0;
    virtual std::vector<std::byte> decompress(std::vector<std::byte> data) = 0;
    virtual int algorithmType() const = 0;

    virtual ~ICompressionAlgorithm() = default;

};

#endif
