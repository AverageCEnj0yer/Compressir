#include "CompressionService.h"

#include "ArchiveEntry.h"
#include "AlgorithmCreator.h"

#include <memory>
#include <utility>
#include <vector>
#include <iostream>

namespace {
CompressionAlgorithm selectAlgorithm(const std::vector<std::byte>& data) {
    if (data.empty()) {
        return CompressionAlgorithm::RLE;
    }

    std::size_t repetitions{0};
    for (std::size_t i = 1; i < data.size(); ++i) {
        if (data[i] == data[i - 1]) {
            ++repetitions;
        }
    }

    // A learning heuristic: prefer RLE when most bytes repeat their predecessor.
    return repetitions > data.size() / 2
        ? CompressionAlgorithm::RLE
        : CompressionAlgorithm::Huffman;
}
}

CompressionService::CompressionService(
        std::unique_ptr<Archiver> archiver,
        std::unique_ptr<IWriter> writer,
        std::unique_ptr<IReader> reader
    )
        : m_archiver{std::move(archiver)},
          m_writer{std::move(writer)},
          m_reader{std::move(reader)}
    {
    }

void CompressionService::compress(const CompressionRequest& request) {
        std::cout << "Starting compression...\n";

        std::vector<ArchiveEntry> entries;

        for (const auto& path : request.files) {
            // TODO: Validate each input file.

            auto data = m_reader->readData(path);

            auto algorithm = request.automaticAlgorithmSelection
                ? selectAlgorithm(data)
                : request.algorithm;
            auto creator = createAlgorithmCreator(algorithm);
            auto compressor = creator->createAlgorithm();
            auto compressed = compressor->compress(data);

            ArchiveEntry entry;
            entry.relativePath = path.filename(); // Simplified.
            entry.originalSize = data.size();
            entry.algorithm = compressor->algorithmType();
            entry.compressedData = std::move(compressed);

            entries.push_back(std::move(entry));
        }

        auto archiveData = m_archiver->pack(entries);

        m_writer->writeData(request.outputPath, archiveData);

        // TODO: Handle errors and report actual results.
        std::cout << "Compression workflow finished.\n";
    }

void CompressionService::decompress(const CompressionRequest& request) {
        std::cout << "Starting decompression...\n";

        for (const auto& archivePath : request.files) {
            auto archiveData = m_reader->readData(archivePath);

            auto entries = m_archiver->unpack(archiveData);

            for (const auto& entry : entries) {
                auto creator = createAlgorithmCreator(entry.algorithm);
                auto compressor = creator->createAlgorithm();
                auto restored = compressor->decompress(
                    entry.compressedData
                );

                // TODO: Safely validate the relative path
                // before writing extracted files.
                auto outputPath =
                    request.outputPath / entry.relativePath;

                m_writer->writeData(outputPath, restored);
            }
        }

        // TODO: Handle errors and report actual results.
        std::cout << "Decompression workflow finished.\n";
    }
