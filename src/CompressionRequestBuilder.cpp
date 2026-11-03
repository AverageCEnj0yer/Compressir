#include "CompressionRequestBuilder.h"

CompressionRequestBuilder& CompressionRequestBuilder::operation(Operation operation) {
    m_request.operation = operation;
    return *this;
}

CompressionRequestBuilder& CompressionRequestBuilder::addFile(const std::filesystem::path& path) {
    m_request.files.push_back(path);
    return *this;
}

CompressionRequestBuilder& CompressionRequestBuilder::output(const std::filesystem::path& path) {
    m_request.outputPath = path;
    return *this;
}

CompressionRequestBuilder& CompressionRequestBuilder::algorithm(CompressionAlgorithm algorithm) {
    m_request.algorithm = algorithm;
    m_request.automaticAlgorithmSelection = false;
    return *this;
}

CompressionRequestBuilder& CompressionRequestBuilder::automaticAlgorithmSelection() {
    m_request.automaticAlgorithmSelection = true;
    return *this;
}

CompressionRequestBuilder& CompressionRequestBuilder::compressionLevel(std::uint8_t level) {
    m_request.compressionLevel = level;
    return *this;
}

CompressionRequest CompressionRequestBuilder::build() const {
    // Request validation remains a TODO in CompressionRequest.
    return m_request;
}
