#include "patterns/AlgorithmDecorator.h"

#include <stdexcept>
#include <utility>

AlgorithmDecorator::AlgorithmDecorator(std::unique_ptr<ICompressionAlgorithm> wrapped)
    : m_wrapped{std::move(wrapped)} {
    if (!m_wrapped) {
        throw std::invalid_argument("Wrapped compression algorithm cannot be null");
    }
}

std::vector<std::byte> AlgorithmDecorator::compress(const std::vector<std::byte>& data) {
    return m_wrapped->compress(data);
}

std::vector<std::byte> AlgorithmDecorator::decompress(const std::vector<std::byte>& data) {
    return m_wrapped->decompress(data);
}

CompressionAlgorithm AlgorithmDecorator::algorithmType() const {
    return m_wrapped->algorithmType();
}
