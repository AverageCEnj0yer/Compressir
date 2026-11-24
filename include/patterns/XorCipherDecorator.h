#ifndef XOR_CIPHER_DECORATOR_H
#define XOR_CIPHER_DECORATOR_H

#include "patterns/AlgorithmDecorator.h"

#include <cstdint>

class XorCipherDecorator : public AlgorithmDecorator {
private:
    std::uint8_t m_key;

    void applyXor(std::vector<std::byte>& buffer) const;

public:
    explicit XorCipherDecorator(std::unique_ptr<ICompressionAlgorithm> wrapped,
                                std::uint8_t key = 0x5A);

    std::vector<std::byte> compress(const std::vector<std::byte>& data) override;
    std::vector<std::byte> decompress(const std::vector<std::byte>& data) override;
};

#endif
