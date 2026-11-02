#ifndef FILE_READER_H
#define FILE_READER_H

#include "IReader.h"

class FileReader : public IReader {
public:
    std::vector<std::byte> readData(
        const std::filesystem::path& path
    ) override;
};

#endif
