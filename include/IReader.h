#ifndef IREADER_H
#define IREADER_H

#include <filesystem>
#include <vector>
#include <cstddef>

class IReader {
public:
    virtual ~IReader() = default;

    virtual std::vector<std::byte> readData(
        const std::filesystem::path& path
    ) = 0;
};

#endif
