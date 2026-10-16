#ifndef COMPRESSIONREQUEST_H
#define COMPRESSIONREQUEST_H

#include <vector>
#include <string>
#include <cstdint>

//later to define enums with possibilities here for compression algorithm, level and format.
class CompressionRequest
{   
    using fileNames = std::vector<std::string>;
    private:
    fileNames m_files{};
    std::string m_outPath{};
    std::string m_compressionAlgorithm{};
    std::uint8_t m_compressionLevel{};
    std::string m_archiveFormat{};

    public:
    CompressionRequest(
        fileNames files,
        std::string outPath,
        std::string compressionAlgorithm,
        uint8_t compressionLevel,
        std::string archiveFormat
    ) : m_files{files}, m_outPath{outPath}, m_compressionAlgorithm{compressionAlgorithm},
        m_compressionLevel{compressionLevel}, m_archiveFormat{archiveFormat} {};

    CompressionRequest() = default; //temporary for testing purposes

    //getters
    const fileNames& getFiles() const { return m_files; }
    const std::string& getOutPath() const { return m_outPath; }
    const std::string& getCompressionAlgorithm() const { return m_compressionAlgorithm; }
    std::uint8_t getCompressionLevel() const { return m_compressionLevel; }
    const std::string& getArchiveFormat() const { return m_archiveFormat; }
};

#endif
