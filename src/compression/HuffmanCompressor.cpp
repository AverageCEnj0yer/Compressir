#include "compression/HuffmanCompressor.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

struct SymbolFreq {
    std::uint8_t symbol{0};
    std::uint32_t freq{0};
};

struct HuffmanNode {
    std::uint64_t freq{0};
    std::uint8_t symbol{0};
    std::uint8_t minSymbol{0};
    int left{-1};
    int right{-1};
};

void appendU16(std::vector<std::byte>& out, std::uint16_t value) {
    for (unsigned shift = 0; shift < 16; shift += 8) {
        out.push_back(static_cast<std::byte>((value >> shift) & 0xFFu));
    }
}

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

std::uint16_t readU16(const std::vector<std::byte>& data, std::size_t& offset) {
    if (offset + 2 > data.size()) {
        throw std::runtime_error("Corrupted Huffman header: unexpected end of data");
    }
    std::uint16_t value = 0;
    for (unsigned shift = 0; shift < 16; shift += 8) {
        value = static_cast<std::uint16_t>(
            value | (static_cast<std::uint16_t>(std::to_integer<std::uint8_t>(data[offset++])) << shift)
        );
    }
    return value;
}

std::uint32_t readU32(const std::vector<std::byte>& data, std::size_t& offset) {
    if (offset + 4 > data.size()) {
        throw std::runtime_error("Corrupted Huffman header: unexpected end of data");
    }
    std::uint32_t value = 0;
    for (unsigned shift = 0; shift < 32; shift += 8) {
        value |= static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(data[offset++])) << shift;
    }
    return value;
}

std::uint64_t readU64(const std::vector<std::byte>& data, std::size_t& offset) {
    if (offset + 8 > data.size()) {
        throw std::runtime_error("Corrupted Huffman header: unexpected end of data");
    }
    std::uint64_t value = 0;
    for (unsigned shift = 0; shift < 64; shift += 8) {
        value |= static_cast<std::uint64_t>(std::to_integer<std::uint8_t>(data[offset++])) << shift;
    }
    return value;
}

int buildTree(const std::vector<SymbolFreq>& entries, std::vector<HuffmanNode>& nodes) {
    nodes.clear();
    nodes.reserve(entries.size() * 2);

    auto cmp = [&nodes](int lhs, int rhs) {
        if (nodes[static_cast<std::size_t>(lhs)].freq != nodes[static_cast<std::size_t>(rhs)].freq) {
            return nodes[static_cast<std::size_t>(lhs)].freq > nodes[static_cast<std::size_t>(rhs)].freq;
        }
        return nodes[static_cast<std::size_t>(lhs)].minSymbol > nodes[static_cast<std::size_t>(rhs)].minSymbol;
    };
    std::priority_queue<int, std::vector<int>, decltype(cmp)> pq(cmp);

    for (const auto& entry : entries) {
        const int idx = static_cast<int>(nodes.size());
        nodes.push_back(HuffmanNode{entry.freq, entry.symbol, entry.symbol, -1, -1});
        pq.push(idx);
    }

    while (pq.size() > 1) {
        const int left = pq.top();
        pq.pop();
        const int right = pq.top();
        pq.pop();

        const auto& leftNode = nodes[static_cast<std::size_t>(left)];
        const auto& rightNode = nodes[static_cast<std::size_t>(right)];
        const int parent = static_cast<int>(nodes.size());
        nodes.push_back(HuffmanNode{
            leftNode.freq + rightNode.freq,
            0,
            std::min(leftNode.minSymbol, rightNode.minSymbol),
            left,
            right
        });
        pq.push(parent);
    }

    return pq.top();
}

void buildCodes(const std::vector<HuffmanNode>& nodes,
                int nodeIdx,
                std::vector<std::uint8_t>& prefix,
                std::array<std::vector<std::uint8_t>, 256>& codes) {
    const auto& node = nodes[static_cast<std::size_t>(nodeIdx)];
    if (node.left == -1 && node.right == -1) {
        codes[node.symbol] = prefix;
        return;
    }
    if (node.left != -1) {
        prefix.push_back(0);
        buildCodes(nodes, node.left, prefix, codes);
        prefix.pop_back();
    }
    if (node.right != -1) {
        prefix.push_back(1);
        buildCodes(nodes, node.right, prefix, codes);
        prefix.pop_back();
    }
}

} // namespace

std::vector<std::byte> HuffmanCompressor::compress(const std::vector<std::byte>& data) {
    if (data.empty()) {
        return {};
    }

    std::array<std::uint32_t, 256> freqs{};
    for (std::byte b : data) {
        ++freqs[std::to_integer<std::uint8_t>(b)];
    }

    std::vector<SymbolFreq> entries;
    for (std::size_t sym = 0; sym < 256; ++sym) {
        if (freqs[sym] > 0) {
            entries.push_back(SymbolFreq{static_cast<std::uint8_t>(sym), freqs[sym]});
        }
    }

    std::vector<std::byte> out;
    appendU64(out, static_cast<std::uint64_t>(data.size()));
    appendU16(out, static_cast<std::uint16_t>(entries.size()));
    for (const auto& entry : entries) {
        out.push_back(static_cast<std::byte>(entry.symbol));
        appendU32(out, entry.freq);
    }

    if (entries.size() == 1) {
        return out;
    }

    std::vector<HuffmanNode> nodes;
    const int root = buildTree(entries, nodes);

    std::array<std::vector<std::uint8_t>, 256> codes;
    std::vector<std::uint8_t> prefix;
    buildCodes(nodes, root, prefix, codes);

    std::uint8_t currentByte = 0;
    unsigned bitCount = 0;
    for (std::byte b : data) {
        const auto sym = std::to_integer<std::uint8_t>(b);
        for (std::uint8_t bit : codes[sym]) {
            currentByte = static_cast<std::uint8_t>((currentByte << 1u) | bit);
            ++bitCount;
            if (bitCount == 8) {
                out.push_back(static_cast<std::byte>(currentByte));
                currentByte = 0;
                bitCount = 0;
            }
        }
    }
    if (bitCount > 0) {
        currentByte = static_cast<std::uint8_t>(currentByte << (8u - bitCount));
        out.push_back(static_cast<std::byte>(currentByte));
    }

    return out;
}

std::vector<std::byte> HuffmanCompressor::decompress(const std::vector<std::byte>& data) {
    if (data.empty()) {
        return {};
    }

    std::size_t offset = 0;
    const std::uint64_t originalSize = readU64(data, offset);
    const std::uint16_t uniqueCount = readU16(data, offset);
    if (uniqueCount == 0 || uniqueCount > 256) {
        throw std::runtime_error("Corrupted Huffman header: invalid symbol count");
    }

    std::vector<SymbolFreq> entries;
    entries.reserve(uniqueCount);
    std::uint64_t freqSum = 0;
    for (std::uint16_t i = 0; i < uniqueCount; ++i) {
        if (offset >= data.size()) {
            throw std::runtime_error("Corrupted Huffman header: truncated frequency table");
        }
        const auto symbol = std::to_integer<std::uint8_t>(data[offset++]);
        const std::uint32_t freq = readU32(data, offset);
        if (freq == 0) {
            throw std::runtime_error("Corrupted Huffman header: zero frequency entry");
        }
        freqSum += freq;
        entries.push_back(SymbolFreq{symbol, freq});
    }

    if (freqSum != originalSize) {
        throw std::runtime_error("Corrupted Huffman header: size mismatch");
    }

    if (entries.size() == 1) {
        return std::vector<std::byte>(static_cast<std::size_t>(originalSize),
                                      static_cast<std::byte>(entries[0].symbol));
    }

    std::vector<HuffmanNode> nodes;
    const int root = buildTree(entries, nodes);

    std::vector<std::byte> out;
    out.reserve(static_cast<std::size_t>(originalSize));

    int currentNode = root;
    while (out.size() < originalSize) {
        if (offset >= data.size()) {
            throw std::runtime_error("Corrupted Huffman stream: unexpected end of bitstream");
        }
        const auto byteVal = std::to_integer<std::uint8_t>(data[offset++]);
        for (int bitIdx = 7; bitIdx >= 0 && out.size() < originalSize; --bitIdx) {
            const unsigned bit = (byteVal >> static_cast<unsigned>(bitIdx)) & 1u;
            currentNode = (bit == 0)
                ? nodes[static_cast<std::size_t>(currentNode)].left
                : nodes[static_cast<std::size_t>(currentNode)].right;

            if (currentNode < 0) {
                throw std::runtime_error("Corrupted Huffman stream: invalid code traversal");
            }

            const auto& node = nodes[static_cast<std::size_t>(currentNode)];
            if (node.left == -1 && node.right == -1) {
                out.push_back(static_cast<std::byte>(node.symbol));
                currentNode = root;
            }
        }
    }

    return out;
}

CompressionAlgorithm HuffmanCompressor::algorithmType() const {
    return CompressionAlgorithm::Huffman;
}
