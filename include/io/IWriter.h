#ifndef IWRITER_H
#define IWRITER_H

#include <filesystem>
#include <vector>
#include <cstddef>

class IWriter {
public:
    virtual ~IWriter() = default;

    virtual bool writeData(
        const std::filesystem::path& path,
        const std::vector<std::byte>& data
    ) = 0;
};

#endif
