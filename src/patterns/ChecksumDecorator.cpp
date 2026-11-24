#include "patterns/ChecksumDecorator.h"

#include <cstdint>
#include <stdexcept>

namespace {

std::uint32_t fnv1a32(const std::vector<std::byte>& data, std::size_t length) {
    std::uint32_t hash = 2166136261u;
    for (std::size_t i = 0; i < length; ++i) {
        hash ^= static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(data[i]));
        hash *= 16777619u;
    }
    return hash;
}

} // namespace

ChecksumDecorator::ChecksumDecorator(std::unique_ptr<ICompressionAlgorithm> wrapped)
    : AlgorithmDecorator{std::move(wrapped)} {
}

std::vector<std::byte> ChecksumDecorator::compress(const std::vector<std::byte>& data) {
    auto result = m_wrapped->compress(data);
    const std::uint32_t checksum = fnv1a32(result, result.size());

    for (unsigned shift = 0; shift < 32; shift += 8) {
        result.push_back(static_cast<std::byte>((checksum >> shift) & 0xFFu));
    }
    return result;
}

std::vector<std::byte> ChecksumDecorator::decompress(const std::vector<std::byte>& data) {
    if (data.size() < 4) {
        throw std::runtime_error("Corrupted data: missing checksum trailer");
    }

    const std::size_t payloadSize = data.size() - 4;
    std::uint32_t expected = 0;
    for (unsigned i = 0; i < 4; ++i) {
        expected |= static_cast<std::uint32_t>(
                        std::to_integer<std::uint8_t>(data[payloadSize + i]))
                    << (i * 8u);
    }

    const std::uint32_t actual = fnv1a32(data, payloadSize);
    if (actual != expected) {
        throw std::runtime_error("Checksum verification failed: compressed data is corrupted");
    }

    std::vector<std::byte> payload(
        data.begin(),
        data.begin() + static_cast<std::ptrdiff_t>(payloadSize)
    );
    return m_wrapped->decompress(payload);
}
