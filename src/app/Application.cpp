#include "app/Application.h"

#include "app/CompressionRequestBuilder.h"
#include "archive/Archiver.h"
#include "patterns/SafeWriterProxy.h"

#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<std::string> tokenizeCommand(std::string_view command) {
    std::istringstream iss{std::string{command}};
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> std::quoted(token)) {
        tokens.push_back(token);
    }
    return tokens;
}

} // namespace

Application::Application(std::string_view command)
    : m_command{command} {
}

CompressionService Application::createService(
    const IIOFactory& ioFactory,
    const std::filesystem::path& allowedWriteDir
) {
    auto guardedWriter = std::make_unique<SafeWriterProxy>(
        ioFactory.createWriter(),
        allowedWriteDir
    );
    return CompressionService{
        std::make_unique<Archiver>(),
        std::move(guardedWriter),
        ioFactory.createReader()
    };
}

void Application::handleCompress(const CompressionRequest& request) {
    FileIOFactory ioFactory;
    const auto allowedDir =
        (request.outputPath.has_parent_path() && !request.outputPath.parent_path().empty())
            ? request.outputPath.parent_path()
            : std::filesystem::current_path();
    auto service = createService(ioFactory, allowedDir);
    service.compress(request);
}

void Application::handleDecompress(const CompressionRequest& request) {
    FileIOFactory ioFactory;
    auto service = createService(ioFactory, request.outputPath);
    service.decompress(request);
}

CompressionRequest Application::buildRequest() {
    const auto tokens = tokenizeCommand(m_command);
    if (tokens.empty()) {
        throw std::invalid_argument("Empty command");
    }

    CompressionRequestBuilder builder;
    if (tokens[0] == "compress") {
        builder.operation(Operation::Compress);
    } else if (tokens[0] == "decompress") {
        builder.operation(Operation::Decompress);
    } else {
        throw std::invalid_argument("Unknown operation: " + tokens[0]);
    }

    for (std::size_t i = 1; i < tokens.size(); ++i) {
        const auto& arg = tokens[i];
        if (arg == "-o" || arg == "--output") {
            if (i + 1 >= tokens.size()) {
                throw std::invalid_argument("Missing path after " + arg);
            }
            builder.output(tokens[++i]);
        } else if (arg == "--algo") {
            if (i + 1 >= tokens.size()) {
                throw std::invalid_argument("Missing algorithm name after --algo");
            }
            const auto& algo = tokens[++i];
            if (algo == "rle") {
                builder.algorithm(CompressionAlgorithm::RLE);
            } else if (algo == "huffman") {
                builder.algorithm(CompressionAlgorithm::Huffman);
            } else if (algo == "auto") {
                builder.automaticAlgorithmSelection();
            } else {
                throw std::invalid_argument("Unknown algorithm: " + algo);
            }
        } else if (arg == "--checksum") {
            builder.checksum(true);
        } else if (arg == "--encrypt") {
            builder.encryption(true);
        } else {
            builder.addFile(arg);
        }
    }

    auto request = builder.build();
    if (request.files.empty()) {
        throw std::invalid_argument("No input files or folders specified");
    }
    if (request.outputPath.empty()) {
        throw std::invalid_argument("Missing output path (-o <path>)");
    }
    return request;
}

void Application::run() {
    auto request = buildRequest();

    switch (request.operation) {
        case Operation::Compress:
            handleCompress(request);
            break;

        case Operation::Decompress:
            handleDecompress(request);
            break;
    }
}
