#include "patterns/FileItem.h"

#include <utility>

FileItem::FileItem(std::filesystem::path sourcePath)
    : m_sourcePath{std::move(sourcePath)},
      m_archivePath{m_sourcePath.filename()} {
}

FileItem::FileItem(std::filesystem::path sourcePath, std::filesystem::path archivePath)
    : m_sourcePath{std::move(sourcePath)},
      m_archivePath{std::move(archivePath)} {
}

std::string FileItem::name() const {
    return m_archivePath.filename().string();
}

std::uint64_t FileItem::totalSize(IReader& reader) const {
    return static_cast<std::uint64_t>(reader.readData(m_sourcePath).size());
}

std::vector<ArchiveInputFile> FileItem::collectFiles() const {
    return {ArchiveInputFile{m_sourcePath, m_archivePath}};
}
