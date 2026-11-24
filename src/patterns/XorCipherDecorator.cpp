#include "patterns/XorCipherDecorator.h"

#include <utility>

XorCipherDecorator::XorCipherDecorator(std::unique_ptr<ICompressionAlgorithm> wrapped,
                                       std::uint8_t key)
    : AlgorithmDecorator{std::move(wrapped)},
      m_key{key} {
}

void XorCipherDecorator::applyXor(std::vector<std::byte>& buffer) const {
    const auto mask = static_cast<std::byte>(m_key);
    for (std::byte& b : buffer) {
        b ^= mask;
    }
}

std::vector<std::byte> XorCipherDecorator::compress(const std::vector<std::byte>& data) {
    auto result = m_wrapped->compress(data);
    applyXor(result);
    return result;
}

std::vector<std::byte> XorCipherDecorator::decompress(const std::vector<std::byte>& data) {
    std::vector<std::byte> unmasked = data;
    applyXor(unmasked);
    return m_wrapped->decompress(unmasked);
}
