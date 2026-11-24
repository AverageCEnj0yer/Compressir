#ifndef SAFE_WRITER_PROXY_H
#define SAFE_WRITER_PROXY_H

#include "io/IWriter.h"

#include <filesystem>
#include <memory>

class SafeWriterProxy : public IWriter {
private:
    std::unique_ptr<IWriter> m_realWriter;
    std::filesystem::path m_allowedBaseDir;

    bool isPathAllowed(const std::filesystem::path& path) const;

public:
    SafeWriterProxy(std::unique_ptr<IWriter> realWriter,
                    std::filesystem::path allowedBaseDir);

    bool writeData(const std::filesystem::path& path,
                   const std::vector<std::byte>& data) override;
};

#endif
