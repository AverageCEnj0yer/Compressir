#ifndef IARCHIVE_ITEM_H
#define IARCHIVE_ITEM_H

#include "io/IReader.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct ArchiveInputFile {
    std::filesystem::path sourcePath{};
    std::filesystem::path archivePath{};
};

class IArchiveItem {
public:
    virtual ~IArchiveItem() = default;

    virtual std::string name() const = 0;
    virtual std::uint64_t totalSize(IReader& reader) const = 0;
    virtual std::vector<ArchiveInputFile> collectFiles() const = 0;
};

#endif
