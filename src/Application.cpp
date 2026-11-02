#include <Application.h>

#include "RLECompressor.h"
#include "HuffmanCompressor.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "Archiver.h"

#include <memory>
#include <iostream>


Application::Application(std::string_view command)
        : m_command{command}
    {
    }


CompressionService Application::createService(
        const CompressionRequest& request
    ) {
        // TODO: Move construction logic into a factory
        // when introducing design patterns.

        std::unique_ptr<ICompressionAlgorithm> compressor;

        switch (request.algorithm) {
            case CompressionAlgorithm::RLE:
                compressor = std::make_unique<RLECompressor>();
                break;

            case CompressionAlgorithm::Huffman:
                compressor = std::make_unique<HuffmanCompressor>();
                break;
        }

        return CompressionService{
            std::make_unique<Archiver>(),
            std::move(compressor),
            std::make_unique<FileWriter>(),
            std::make_unique<FileReader>()
        };
    }

void Application::handleCompress(const CompressionRequest& request) {
    auto service = createService(request);
    service.compress(request);
}

void Application::handleDecompress(const CompressionRequest& request) {
    auto service = createService(request);
    service.decompress(request);
}

CompressionRequest Application::buildRequest() {
    // TODO: Parse m_command.
    // TODO: Extract operation, files, output path,
    //       algorithm and compression settings.
    // TODO: Validate request syntax and options.
    // TODO: Report invalid arguments.

    std::cout << "Building request...\n";

    return CompressionRequest{}; // Placeholder.
}

void Application::run() {
    auto request = buildRequest();

    // TODO: Handle invalid requests before execution.

    switch (request.operation) {
        case Operation::Compress:
            handleCompress(request);
            break;

        case Operation::Decompress:
            handleDecompress(request);
            break;
    }
}
