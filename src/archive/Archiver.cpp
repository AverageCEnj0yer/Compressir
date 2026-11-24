#include "archive/Archiver.h"

#include <array>
#include <cstdint>
#include <stdexcept>

namespace {

constexpr std::array<std::byte, 4> kMagic{
    std::byte{'C'}, std::byte{'M'}, std::byte{'P'}, std::byte{'R'}
};
constexpr std::uint8_t kFormatVersion = 1;
constexpr std::uint8_t kFlagChecksum = 1u << 0;
constexpr std::uint8_t kFlagEncrypted = 1u << 1;

void appendU32(std::vector<std::byte>& out, std::uint32_t value) {
    for (unsigned shift = 0; shift < 32; shift += 8) {
        out.push_back(static_cast<std::byte>((value >> shift) & 0xFFu));
    }
}

void appendU64(std::vector<std::byte>& out, std::uint64_t value) {
    for (unsigned shift = 0; shift < 64; shift += 8) {
        out.push_back(static_cast<std::byte>((value >> shift) & 0xFFu));
    }
}

std::uint8_t readU8(const std::vector<std::byte>& data, std::size_t& offset) {
    if (offset >= data.size()) {
        throw std::runtime_error("Corrupted archive: unexpected end of data");
    }
    return std::to_integer<std::uint8_t>(data[offset++]);
}

std::uint32_t readU32(const std::vector<std::byte>& data, std::size_t& offset) {
    if (offset + 4 > data.size()) {
        throw std::runtime_error("Corrupted archive: unexpected end of data");
    }
    std::uint32_t value = 0;
    for (unsigned shift = 0; shift < 32; shift += 8) {
        value |= static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(data[offset++])) << shift;
    }
    return value;
}

std::uint64_t readU64(const std::vector<std::byte>& data, std::size_t& offset) {
    if (offset + 8 > data.size()) {
        throw std::runtime_error("Corrupted archive: unexpected end of data");
    }
    std::uint64_t value = 0;
    for (unsigned shift = 0; shift < 64; shift += 8) {
        value |= static_cast<std::uint64_t>(std::to_integer<std::uint8_t>(data[offset++])) << shift;
    }
    return value;
}

} // namespace

std::vector<std::byte> Archiver::pack(const std::vector<ArchiveEntry>& entries) {
    std::vector<std::byte> out;
    out.insert(out.end(), kMagic.begin(), kMagic.end());
    out.push_back(static_cast<std::byte>(kFormatVersion));
    appendU32(out, static_cast<std::uint32_t>(entries.size()));

    for (const auto& entry : entries) {
        const std::string pathStr = entry.relativePath.generic_string();
        appendU32(out, static_cast<std::uint32_t>(pathStr.size()));
        for (char ch : pathStr) {
            out.push_back(static_cast<std::byte>(static_cast<unsigned char>(ch)));
        }

        appendU64(out, entry.originalSize);
        out.push_back(static_cast<std::byte>(static_cast<std::uint8_t>(entry.algorithm)));

        std::uint8_t flags = 0;
        if (entry.hasChecksum) {
            flags |= kFlagChecksum;
        }
        if (entry.isEncrypted) {
            flags |= kFlagEncrypted;
        }
        out.push_back(static_cast<std::byte>(flags));

        appendU64(out, static_cast<std::uint64_t>(entry.compressedData.size()));
        out.insert(out.end(), entry.compressedData.begin(), entry.compressedData.end());
    }

    return out;
}

std::vector<ArchiveEntry> Archiver::unpack(const std::vector<std::byte>& archiveData) {
    if (archiveData.size() < 9) {
        throw std::runtime_error("Corrupted archive: header too short");
    }

    for (std::size_t i = 0; i < kMagic.size(); ++i) {
        if (archiveData[i] != kMagic[i]) {
            throw std::runtime_error("Invalid archive: bad magic header");
        }
    }

    std::size_t offset = kMagic.size();
    const std::uint8_t version = readU8(archiveData, offset);
    if (version != kFormatVersion) {
        throw std::runtime_error("Unsupported archive version");
    }

    const std::uint32_t entryCount = readU32(archiveData, offset);
    std::vector<ArchiveEntry> entries;
    entries.reserve(entryCount);

    for (std::uint32_t i = 0; i < entryCount; ++i) {
        const std::uint32_t pathLen = readU32(archiveData, offset);
        if (offset + pathLen > archiveData.size()) {
            throw std::runtime_error("Corrupted archive: invalid path length");
        }
        std::string pathStr;
        pathStr.reserve(pathLen);
        for (std::uint32_t j = 0; j < pathLen; ++j) {
            pathStr.push_back(static_cast<char>(std::to_integer<unsigned char>(archiveData[offset++])));
        }

        ArchiveEntry entry;
        entry.relativePath = std::filesystem::path(pathStr);
        entry.originalSize = readU64(archiveData, offset);

        const std::uint8_t algoByte = readU8(archiveData, offset);
        if (algoByte != static_cast<std::uint8_t>(CompressionAlgorithm::RLE) &&
            algoByte != static_cast<std::uint8_t>(CompressionAlgorithm::Huffman)) {
            throw std::runtime_error("Corrupted archive: unknown compression algorithm");
        }
        entry.algorithm = static_cast<CompressionAlgorithm>(algoByte);

        const std::uint8_t flags = readU8(archiveData, offset);
        entry.hasChecksum = (flags & kFlagChecksum) != 0;
        entry.isEncrypted = (flags & kFlagEncrypted) != 0;

        const std::uint64_t compressedSize = readU64(archiveData, offset);
        if (compressedSize > archiveData.size() - offset) {
            throw std::runtime_error("Corrupted archive: truncated entry payload");
        }

        const auto payloadSize = static_cast<std::size_t>(compressedSize);
        entry.compressedData.assign(
            archiveData.begin() + static_cast<std::ptrdiff_t>(offset),
            archiveData.begin() + static_cast<std::ptrdiff_t>(offset + payloadSize)
        );
        offset += payloadSize;

        entries.push_back(std::move(entry));
    }

    if (offset != archiveData.size()) {
        throw std::runtime_error("Corrupted archive: unexpected trailing bytes");
    }

    return entries;
}
