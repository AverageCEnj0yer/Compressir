#include "CompressionService.h"

#include "ArchiveEntry.h"

#include <memory>
#include <utility>
#include <vector>
#include <iostream>

CompressionService::CompressionService(
        std::unique_ptr<Archiver> archiver,
        std::unique_ptr<ICompressionAlgorithm> compressor,
        std::unique_ptr<IWriter> writer,
        std::unique_ptr<IReader> reader
    )
        : m_archiver{std::move(archiver)},
          m_compressor{std::move(compressor)},
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

            auto compressed = m_compressor->compress(data);

            ArchiveEntry entry;
            entry.relativePath = path.filename(); // Simplified.
            entry.originalSize = data.size();
            entry.algorithm = m_compressor->algorithmType();
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
                // TODO: Select decoder using entry.algorithm.
                // Currently assumes the injected compressor
                // is appropriate for every archive entry.

                auto restored = m_compressor->decompress(
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
