#ifndef ALGORITHM_DECORATOR_H
#define ALGORITHM_DECORATOR_H

#include "compression/ICompressionAlgorithm.h"

#include <memory>

class AlgorithmDecorator : public ICompressionAlgorithm {
protected:
    std::unique_ptr<ICompressionAlgorithm> m_wrapped;

public:
    explicit AlgorithmDecorator(std::unique_ptr<ICompressionAlgorithm> wrapped);

    std::vector<std::byte> compress(const std::vector<std::byte>& data) override;
    std::vector<std::byte> decompress(const std::vector<std::byte>& data) override;
    CompressionAlgorithm algorithmType() const override;
};

#endif
