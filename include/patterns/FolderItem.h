#ifndef FOLDER_ITEM_H
#define FOLDER_ITEM_H

#include "patterns/IArchiveItem.h"

#include <memory>
#include <string>
#include <vector>

class FolderItem : public IArchiveItem {
private:
    std::string m_name;
    std::vector<std::unique_ptr<IArchiveItem>> m_children;

public:
    explicit FolderItem(std::string name);

    void add(std::unique_ptr<IArchiveItem> item);

    std::string name() const override;
    std::uint64_t totalSize(IReader& reader) const override;
    std::vector<ArchiveInputFile> collectFiles() const override;
};

std::unique_ptr<IArchiveItem> buildArchiveItemFromPath(const std::filesystem::path& path);

#endif
