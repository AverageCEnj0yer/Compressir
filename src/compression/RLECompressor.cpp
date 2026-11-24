#include "compression/RLECompressor.h"

#include <cstdint>
#include <stdexcept>

std::vector<std::byte> RLECompressor::compress(const std::vector<std::byte>& data) {
    std::vector<std::byte> out;
    out.reserve(data.size());

    std::size_t i = 0;
    while (i < data.size()) {
        if (i + 1 < data.size() && data[i] == data[i + 1]) {
            std::size_t runLen = 2;
            while (i + runLen < data.size() && runLen < 129 && data[i + runLen] == data[i]) {
                ++runLen;
            }
            const auto ctrl = static_cast<std::uint8_t>(0x80u | static_cast<std::uint8_t>(runLen - 2u));
            out.push_back(static_cast<std::byte>(ctrl));
            out.push_back(data[i]);
            i += runLen;
        } else {
            const std::size_t litStart = i;
            std::size_t litLen = 0;
            while (i + litLen < data.size() && litLen < 128) {
                if (litLen > 0 && i + litLen + 1 < data.size() &&
                    data[i + litLen] == data[i + litLen + 1]) {
                    break;
                }
                ++litLen;
            }
            const auto ctrl = static_cast<std::uint8_t>(litLen - 1u);
            out.push_back(static_cast<std::byte>(ctrl));
            for (std::size_t k = 0; k < litLen; ++k) {
                out.push_back(data[litStart + k]);
            }
            i += litLen;
        }
    }

    return out;
}

std::vector<std::byte> RLECompressor::decompress(const std::vector<std::byte>& data) {
    std::vector<std::byte> out;

    std::size_t i = 0;
    while (i < data.size()) {
        const auto ctrl = std::to_integer<std::uint8_t>(data[i++]);
        if ((ctrl & 0x80u) != 0) {
            const std::size_t runLen = static_cast<std::size_t>(ctrl & 0x7Fu) + 2u;
            if (i >= data.size()) {
                throw std::runtime_error("Corrupted RLE stream: missing repeated byte");
            }
            const std::byte value = data[i++];
            out.insert(out.end(), runLen, value);
        } else {
            const std::size_t litLen = static_cast<std::size_t>(ctrl) + 1u;
            if (i + litLen > data.size()) {
                throw std::runtime_error("Corrupted RLE stream: truncated literal run");
            }
            out.insert(out.end(),
                       data.begin() + static_cast<std::ptrdiff_t>(i),
                       data.begin() + static_cast<std::ptrdiff_t>(i + litLen));
            i += litLen;
        }
    }

    return out;
}

CompressionAlgorithm RLECompressor::algorithmType() const {
    return CompressionAlgorithm::RLE;
}
