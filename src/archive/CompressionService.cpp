#include "archive/CompressionService.h"

#include "compression/AlgorithmCreator.h"
#include "models/ArchiveEntry.h"
#include "patterns/ChecksumDecorator.h"
#include "patterns/FolderItem.h"
#include "patterns/XorCipherDecorator.h"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

CompressionAlgorithm selectAlgorithm(const std::vector<std::byte>& data) {
    if (data.empty()) {
        return CompressionAlgorithm::RLE;
    }

    std::size_t repetitions = 0;
    for (std::size_t i = 1; i < data.size(); ++i) {
        if (data[i] == data[i - 1]) {
            ++repetitions;
        }
    }

    return repetitions > data.size() / 2
        ? CompressionAlgorithm::RLE
        : CompressionAlgorithm::Huffman;
}

std::unique_ptr<ICompressionAlgorithm> wrapWithDecorators(
    std::unique_ptr<ICompressionAlgorithm> base,
    bool enableChecksum,
    bool enableEncryption,
    std::uint8_t encryptionKey
) {
    if (enableChecksum) {
        base = std::make_unique<ChecksumDecorator>(std::move(base));
    }
    if (enableEncryption) {
        base = std::make_unique<XorCipherDecorator>(std::move(base), encryptionKey);
    }
    return base;
}

const char* algorithmName(CompressionAlgorithm algo) {
    return algo == CompressionAlgorithm::RLE ? "RLE" : "Huffman";
}

} // namespace

CompressionService::CompressionService(
    std::unique_ptr<Archiver> archiver,
    std::unique_ptr<IWriter> writer,
    std::unique_ptr<IReader> reader
)
    : m_archiver{std::move(archiver)},
      m_writer{std::move(writer)},
      m_reader{std::move(reader)} {
}

void CompressionService::compress(const CompressionRequest& request) {
    if (request.files.empty()) {
        throw std::invalid_argument("Compression request requires at least one input path");
    }

    FolderItem root{""};
    for (const auto& path : request.files) {
        root.add(buildArchiveItemFromPath(path));
    }

    compress(root, request);
}

void CompressionService::compress(const IArchiveItem& root, const CompressionRequest& request) {
    if (request.outputPath.empty()) {
        throw std::invalid_argument("Compression request requires an output archive path");
    }

    const auto inputFiles = root.collectFiles();
    if (inputFiles.empty()) {
        throw std::runtime_error("No files found to compress");
    }

    std::vector<ArchiveEntry> entries;
    entries.reserve(inputFiles.size());
    std::uint64_t totalOriginalBytes = 0;

    for (const auto& file : inputFiles) {
        auto data = m_reader->readData(file.sourcePath);
        totalOriginalBytes += data.size();

        const auto algorithmType = request.automaticAlgorithmSelection
            ? selectAlgorithm(data)
            : request.algorithm;

        auto creator = createAlgorithmCreator(algorithmType);
        auto compressor = wrapWithDecorators(
            creator->createAlgorithm(),
            request.enableChecksum,
            request.enableEncryption,
            request.encryptionKey
        );

        auto compressed = compressor->compress(data);

        std::cout << "  [" << algorithmName(compressor->algorithmType()) << "] "
                  << file.archivePath.generic_string()
                  << ": " << data.size() << " B -> " << compressed.size() << " B\n";

        ArchiveEntry entry;
        entry.relativePath = file.archivePath;
        entry.originalSize = data.size();
        entry.algorithm = compressor->algorithmType();
        entry.hasChecksum = request.enableChecksum;
        entry.isEncrypted = request.enableEncryption;
        entry.compressedData = std::move(compressed);

        entries.push_back(std::move(entry));
    }

    auto archiveData = m_archiver->pack(entries);
    if (!m_writer->writeData(request.outputPath, archiveData)) {
        throw std::runtime_error("Failed to write archive: " + request.outputPath.string());
    }

    std::cout << "Packed " << entries.size() << " file(s): "
              << totalOriginalBytes << " B -> " << archiveData.size() << " B ("
              << request.outputPath.string() << ")\n";
}

void CompressionService::decompress(const CompressionRequest& request) {
    if (request.files.empty()) {
        throw std::invalid_argument("Decompression request requires an input archive path");
    }
    if (request.outputPath.empty()) {
        throw std::invalid_argument("Decompression request requires an output directory");
    }

    for (const auto& archivePath : request.files) {
        auto archiveData = m_reader->readData(archivePath);
        auto entries = m_archiver->unpack(archiveData);

        for (const auto& entry : entries) {
            auto creator = createAlgorithmCreator(entry.algorithm);
            auto decompressor = wrapWithDecorators(
                creator->createAlgorithm(),
                entry.hasChecksum,
                entry.isEncrypted,
                request.encryptionKey
            );

            auto restored = decompressor->decompress(entry.compressedData);
            if (restored.size() != entry.originalSize) {
                throw std::runtime_error(
                    "Decompressed size mismatch for entry: " + entry.relativePath.generic_string()
                );
            }

            const auto outputPath = request.outputPath / entry.relativePath;
            if (!m_writer->writeData(outputPath, restored)) {
                throw std::runtime_error(
                    "Failed or blocked writing extracted file: " + outputPath.string()
                );
            }

            std::cout << "  [Extracted] " << entry.relativePath.generic_string()
                      << " (" << restored.size() << " B)\n";
        }
    }
}
