#ifndef CHECKSUM_DECORATOR_H
#define CHECKSUM_DECORATOR_H

#include "patterns/AlgorithmDecorator.h"

class ChecksumDecorator : public AlgorithmDecorator {
public:
    explicit ChecksumDecorator(std::unique_ptr<ICompressionAlgorithm> wrapped);

    std::vector<std::byte> compress(const std::vector<std::byte>& data) override;
    std::vector<std::byte> decompress(const std::vector<std::byte>& data) override;
};

#endif
