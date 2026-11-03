#ifndef ALGORITHM_CREATOR_H
#define ALGORITHM_CREATOR_H

#include "ICompressionAlgorithm.h"
#include <memory>

class AlgorithmCreator {
public:
    virtual ~AlgorithmCreator() = default;
    virtual std::unique_ptr<ICompressionAlgorithm> createAlgorithm() const = 0;
};

class RLECreator : public AlgorithmCreator {
public:
    std::unique_ptr<ICompressionAlgorithm> createAlgorithm() const override;
};

class HuffmanCreator : public AlgorithmCreator {
public:
    std::unique_ptr<ICompressionAlgorithm> createAlgorithm() const override;
};

std::unique_ptr<AlgorithmCreator> createAlgorithmCreator(CompressionAlgorithm algorithm);

#endif
