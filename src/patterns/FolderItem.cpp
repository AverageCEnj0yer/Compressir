#include "patterns/FolderItem.h"
#include "patterns/FileItem.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace {

std::string extractFolderName(const std::filesystem::path& path) {
    auto clean = path.lexically_normal();
    while (!clean.empty() && (clean.filename().empty() || clean.filename() == ".")) {
        auto parent = clean.parent_path();
        if (parent == clean) {
            break;
        }
        clean = parent;
    }
    std::string result = clean.filename().string();
    return result.empty() ? "root" : result;
}

} // namespace

FolderItem::FolderItem(std::string name)
    : m_name{std::move(name)} {
}

void FolderItem::add(std::unique_ptr<IArchiveItem> item) {
    if (!item) {
        throw std::invalid_argument("Cannot add null archive item to folder");
    }
    m_children.push_back(std::move(item));
}

std::string FolderItem::name() const {
    return m_name;
}

std::uint64_t FolderItem::totalSize(IReader& reader) const {
    std::uint64_t sum = 0;
    for (const auto& child : m_children) {
        sum += child->totalSize(reader);
    }
    return sum;
}

std::vector<ArchiveInputFile> FolderItem::collectFiles() const {
    std::vector<ArchiveInputFile> collected;
    for (const auto& child : m_children) {
        auto childFiles = child->collectFiles();
        for (auto& file : childFiles) {
            if (!m_name.empty()) {
                file.archivePath = std::filesystem::path{m_name} / file.archivePath;
            }
            collected.push_back(std::move(file));
        }
    }
    return collected;
}

std::unique_ptr<IArchiveItem> buildArchiveItemFromPath(const std::filesystem::path& path) {
    if (std::filesystem::is_directory(path)) {
        auto folder = std::make_unique<FolderItem>(extractFolderName(path));
        std::vector<std::filesystem::path> childPaths;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            childPaths.push_back(entry.path());
        }
        std::sort(childPaths.begin(), childPaths.end());
        for (const auto& childPath : childPaths) {
            folder->add(buildArchiveItemFromPath(childPath));
        }
        return folder;
    }

    return std::make_unique<FileItem>(path);
}
