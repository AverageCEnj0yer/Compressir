#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include "IWriter.h"

class FileWriter : public IWriter {
public:
    bool writeData(
        const std::filesystem::path& path,
        [[maybe_unused]] const std::vector<std::byte>& data
    ) override;
};

#endif
