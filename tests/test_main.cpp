#include "app/CompressionRequestBuilder.h"
#include "archive/Archiver.h"
#include "archive/CompressionService.h"
#include "compression/AlgorithmCreator.h"
#include "compression/HuffmanCompressor.h"
#include "compression/RLECompressor.h"
#include "io/IOFactory.h"
#include "patterns/ChecksumDecorator.h"
#include "patterns/FileItem.h"
#include "patterns/FolderItem.h"
#include "patterns/SafeWriterProxy.h"
#include "patterns/XorCipherDecorator.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<std::byte> toBytes(const std::string& text) {
    std::vector<std::byte> bytes;
    bytes.reserve(text.size());
    for (char ch : text) {
        bytes.push_back(static_cast<std::byte>(static_cast<unsigned char>(ch)));
    }
    return bytes;
}

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("Assertion failed: " + message);
    }
}

void testCompositeLeafAndGroupUniformInterface() {
    MemoryIOFactory io;
    auto writer = io.createWriter();
    auto reader = io.createReader();

    const auto aBytes = toBytes("alpha data");
    const auto bBytes = toBytes("beta nested data");
    writer->writeData("mem/a.txt", aBytes);
    writer->writeData("mem/sub/b.txt", bBytes);

    FileItem leaf{"mem/a.txt"};
    auto root = std::make_unique<FolderItem>("docs");
    root->add(std::make_unique<FileItem>("mem/a.txt"));

    auto sub = std::make_unique<FolderItem>("sub");
    sub->add(std::make_unique<FileItem>("mem/sub/b.txt"));
    root->add(std::move(sub));

    const IArchiveItem& leafRef = leaf;
    const IArchiveItem& groupRef = *root;

    require(leafRef.totalSize(*reader) == aBytes.size(), "Leaf totalSize mismatch");
    require(leafRef.collectFiles().size() == 1, "Leaf should collect 1 file");
    require(leafRef.collectFiles()[0].archivePath == "a.txt", "Leaf archivePath mismatch");

    require(groupRef.totalSize(*reader) == aBytes.size() + bBytes.size(), "Group totalSize mismatch");
    const auto collected = groupRef.collectFiles();
    require(collected.size() == 2, "Group should collect 2 files");
    require(collected[0].archivePath == std::filesystem::path{"docs/a.txt"}, "First nested path mismatch");
    require(collected[1].archivePath == std::filesystem::path{"docs/sub/b.txt"}, "Second nested path mismatch");
}

void testCompositeWithCompressionService() {
    MemoryIOFactory io;
    auto setupWriter = io.createWriter();
    const auto payload1 = toBytes(std::string(300, 'X'));
    const auto payload2 = toBytes(std::string(300, 'Y') + "hello world hello world hello world");
    setupWriter->writeData("src/one.txt", payload1);
    setupWriter->writeData("src/inner/two.txt", payload2);

    auto folder = std::make_unique<FolderItem>("bundle");
    folder->add(std::make_unique<FileItem>("src/one.txt"));
    auto inner = std::make_unique<FolderItem>("inner");
    inner->add(std::make_unique<FileItem>("src/inner/two.txt"));
    folder->add(std::move(inner));

    CompressionService compressService{
        std::make_unique<Archiver>(),
        std::make_unique<SafeWriterProxy>(io.createWriter(), "out"),
        io.createReader()
    };

    const auto compressReq = CompressionRequestBuilder{}
        .operation(Operation::Compress)
        .output("out/test.cmpr")
        .automaticAlgorithmSelection()
        .build();
    compressService.compress(*folder, compressReq);

    CompressionService decompressService{
        std::make_unique<Archiver>(),
        std::make_unique<SafeWriterProxy>(io.createWriter(), "extracted"),
        io.createReader()
    };
    const auto decompressReq = CompressionRequestBuilder{}
        .operation(Operation::Decompress)
        .addFile("out/test.cmpr")
        .output("extracted")
        .build();
    decompressService.decompress(decompressReq);

    auto verifyReader = io.createReader();
    require(verifyReader->readData("extracted/bundle/one.txt") == payload1, "Extracted leaf mismatch");
    require(verifyReader->readData("extracted/bundle/inner/two.txt") == payload2, "Extracted nested mismatch");
}

void testDecoratorStackingAnyOrder() {
    const auto original = toBytes(
        "Pattern stacking test: repeating text repeating text repeating text AAAAAAAAABBBBBBBB"
    );

    // Order 1: Checksum(Xor(Huffman))
    std::unique_ptr<ICompressionAlgorithm> stack1 = std::make_unique<ChecksumDecorator>(
        std::make_unique<XorCipherDecorator>(
            std::make_unique<HuffmanCompressor>(), 0x5A
        )
    );
    const auto compressed1 = stack1->compress(original);
    require(stack1->decompress(compressed1) == original, "Order 1 (Checksum(Xor(Huffman))) round-trip failed");

    // Order 2: Xor(Checksum(Huffman))
    std::unique_ptr<ICompressionAlgorithm> stack2 = std::make_unique<XorCipherDecorator>(
        std::make_unique<ChecksumDecorator>(
            std::make_unique<HuffmanCompressor>()
        ),
        0x5A
    );
    const auto compressed2 = stack2->compress(original);
    require(compressed1 != compressed2, "Different decorator orders should produce distinct wrapped bytes");
    require(stack2->decompress(compressed2) == original, "Order 2 (Xor(Checksum(Huffman))) round-trip failed");

    // Also verify RLE with stacked decorators
    std::unique_ptr<ICompressionAlgorithm> rleStack = std::make_unique<ChecksumDecorator>(
        std::make_unique<XorCipherDecorator>(
            std::make_unique<RLECompressor>(), 0x33
        )
    );
    const auto rleCompressed = rleStack->compress(original);
    require(rleStack->decompress(rleCompressed) == original, "Stacked RLE decorator round-trip failed");
}

void testDecoratorChecksumDetectsCorruption() {
    const auto original = toBytes("Important payload that must not be silently corrupted!");
    std::unique_ptr<ICompressionAlgorithm> decorated = std::make_unique<ChecksumDecorator>(
        std::make_unique<XorCipherDecorator>(
            std::make_unique<HuffmanCompressor>(), 0x5A
        )
    );

    auto corrupted = decorated->compress(original);
    corrupted[0] ^= std::byte{0x01};

    bool threw = false;
    try {
        (void)decorated->decompress(corrupted);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    require(threw, "ChecksumDecorator should throw on corrupted data");
}

void testProxyBlocksUnauthorizedAccess() {
    MemoryIOFactory io;
    int realCalls = 0;

    class CountingWriter : public IWriter {
    public:
        int& calls;
        explicit CountingWriter(int& c) : calls{c} {}
        bool writeData(const std::filesystem::path&, const std::vector<std::byte>&) override {
            ++calls;
            return true;
        }
    };

    SafeWriterProxy proxy{std::make_unique<CountingWriter>(realCalls), "sandbox/out"};
    const auto data = toBytes("secret");

    require(!proxy.writeData("sandbox/out/../escape.txt", data), "Proxy must block .. traversal");
    require(!proxy.writeData("/etc/passwd", data), "Proxy must block absolute outside path");
    require(!proxy.writeData("sandbox/out_evil/file.txt", data), "Proxy must block prefix sibling path");
    require(!proxy.writeData("sandbox/out", data), "Proxy must block writing directly to base dir path");
    require(!proxy.writeData("", data), "Proxy must block empty path");
    require(realCalls == 0, "Blocked writes must never reach real writer");
}

void testProxyPassesAuthorizedCallsUnchanged() {
    MemoryIOFactory io;
    SafeWriterProxy proxy{io.createWriter(), "sandbox/out"};
    const auto expected = toBytes("Authorized archive entry payload");

    require(proxy.writeData("sandbox/out/hello.txt", expected), "Proxy should allow direct child write");
    require(proxy.writeData("sandbox/out/sub/deep.txt", expected), "Proxy should allow nested child write");

    auto reader = io.createReader();
    require(reader->readData("sandbox/out/hello.txt") == expected, "Authorized payload altered");
    require(reader->readData("sandbox/out/sub/deep.txt") == expected, "Nested authorized payload altered");
}

void testRleRoundTripAndCompression() {
    RLECompressor rle;
    require(rle.compress({}).empty(), "Empty RLE compress should be empty");
    require(rle.decompress({}).empty(), "Empty RLE decompress should be empty");

    const auto single = toBytes("Z");
    require(rle.decompress(rle.compress(single)) == single, "Single-byte RLE failed");

    const auto mixed = toBytes("ABCDEEEEEFGHIJJJJJJJJJJKL");
    require(rle.decompress(rle.compress(mixed)) == mixed, "Mixed RLE failed");

    const auto repetitive = toBytes(std::string(2000, 'A') + std::string(1000, 'B'));
    const auto compressed = rle.compress(repetitive);
    require(compressed.size() < repetitive.size() / 10, "RLE did not compress repetitive data");
    require(rle.decompress(compressed) == repetitive, "Repetitive RLE round-trip failed");
}

void testHuffmanRoundTripAndCompression() {
    HuffmanCompressor huffman;
    require(huffman.compress({}).empty(), "Empty Huffman compress should be empty");
    require(huffman.decompress({}).empty(), "Empty Huffman decompress should be empty");

    const auto singleSymbol = toBytes(std::string(500, 'Q'));
    const auto singleCompressed = huffman.compress(singleSymbol);
    require(singleCompressed.size() < singleSymbol.size(), "Single-symbol Huffman should shrink");
    require(huffman.decompress(singleCompressed) == singleSymbol, "Single-symbol Huffman failed");

    std::string text;
    for (int i = 0; i < 60; ++i) {
        text += "Design patterns help structure clean object-oriented systems.\n";
    }
    const auto textBytes = toBytes(text);
    const auto textCompressed = huffman.compress(textBytes);
    require(textCompressed.size() < textBytes.size(), "Huffman did not shrink repetitive text");
    require(huffman.decompress(textCompressed) == textBytes, "Huffman text round-trip failed");

    std::vector<std::byte> allBytes;
    allBytes.reserve(512);
    for (int rep = 0; rep < 2; ++rep) {
        for (int b = 0; b < 256; ++b) {
            allBytes.push_back(static_cast<std::byte>(b));
        }
    }
    require(huffman.decompress(huffman.compress(allBytes)) == allBytes, "Huffman 256-byte binary round-trip failed");
}

void testEndToEndPatternsInteractAndBlockZipSlip() {
    MemoryIOFactory io;
    auto setupWriter = io.createWriter();
    const std::string docText = std::string(40, 'A') +
        "Compressir combines Composite, Decorator, and Proxy patterns.\n" +
        "Compressir combines Composite, Decorator, and Proxy patterns.\n";
    const auto docBytes = toBytes(docText);
    setupWriter->writeData("input/readme.txt", docBytes);

    // Full pipeline: Builder + Factory Method + Abstract Factory + Composite + Decorator + Proxy
    CompressionService packService{
        std::make_unique<Archiver>(),
        std::make_unique<SafeWriterProxy>(io.createWriter(), "archives"),
        io.createReader()
    };

    const auto packReq = CompressionRequestBuilder{}
        .operation(Operation::Compress)
        .output("archives/bundle.cmpr")
        .algorithm(CompressionAlgorithm::Huffman)
        .checksum(true)
        .encryption(true, 0x5A)
        .build();

    FileItem singleFile{"input/readme.txt"};
    packService.compress(singleFile, packReq);

    // Now test decompression blocks a malicious archive containing Zip-Slip relativePath
    ArchiveEntry evilEntry;
    evilEntry.relativePath = "../../evil.txt";
    evilEntry.originalSize = 4;
    evilEntry.algorithm = CompressionAlgorithm::RLE;
    evilEntry.compressedData = RLECompressor{}.compress(toBytes("evil"));

    Archiver archiver;
    setupWriter->writeData("archives/malicious.cmpr", archiver.pack({evilEntry}));

    CompressionService extractService{
        std::make_unique<Archiver>(),
        std::make_unique<SafeWriterProxy>(io.createWriter(), "safe_extract"),
        io.createReader()
    };

    const auto evilReq = CompressionRequestBuilder{}
        .operation(Operation::Decompress)
        .addFile("archives/malicious.cmpr")
        .output("safe_extract")
        .build();

    bool blocked = false;
    try {
        extractService.decompress(evilReq);
    } catch (const std::runtime_error&) {
        blocked = true;
    }
    require(blocked, "Zip-Slip extraction should be blocked by SafeWriterProxy");
}

} // namespace

int main() {
    struct TestCase {
        const char* name;
        std::function<void()> fn;
    };

    const std::vector<TestCase> tests{
        {"composite_leaf_and_group_share_interface", testCompositeLeafAndGroupUniformInterface},
        {"composite_integrates_with_compression_service", testCompositeWithCompressionService},
        {"decorator_stackable_in_any_order", testDecoratorStackingAnyOrder},
        {"decorator_checksum_detects_corruption", testDecoratorChecksumDetectsCorruption},
        {"proxy_blocks_unauthorized_access", testProxyBlocksUnauthorizedAccess},
        {"proxy_passes_authorized_calls_unchanged", testProxyPassesAuthorizedCallsUnchanged},
        {"rle_round_trip_and_compression", testRleRoundTripAndCompression},
        {"huffman_round_trip_and_compression", testHuffmanRoundTripAndCompression},
        {"end_to_end_patterns_interact_and_block_zip_slip", testEndToEndPatternsInteractAndBlockZipSlip},
    };

    int failures = 0;
    for (const auto& test : tests) {
        try {
            test.fn();
            std::cout << "[PASS] " << test.name << '\n';
        } catch (const std::exception& ex) {
            ++failures;
            std::cerr << "[FAIL] " << test.name << ": " << ex.what() << '\n';
        }
    }

    std::cout << "\nRan " << tests.size() << " tests, "
              << (tests.size() - static_cast<std::size_t>(failures)) << " passed, "
              << failures << " failed.\n";
    return failures == 0 ? 0 : 1;
}
