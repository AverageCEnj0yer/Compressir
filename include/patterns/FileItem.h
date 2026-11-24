#ifndef FILE_ITEM_H
#define FILE_ITEM_H

#include "patterns/IArchiveItem.h"

#include <filesystem>

class FileItem : public IArchiveItem {
private:
    std::filesystem::path m_sourcePath;
    std::filesystem::path m_archivePath;

public:
    explicit FileItem(std::filesystem::path sourcePath);
    FileItem(std::filesystem::path sourcePath, std::filesystem::path archivePath);

    std::string name() const override;
    std::uint64_t totalSize(IReader& reader) const override;
    std::vector<ArchiveInputFile> collectFiles() const override;
};

#endif
