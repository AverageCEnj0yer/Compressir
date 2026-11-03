#include "IOFactory.h"
#include "FileReader.h"
#include "FileWriter.h"
#include <utility>

MemoryReader::MemoryReader(std::shared_ptr<MemoryStorage> storage)
    : m_storage{std::move(storage)} {
}

std::vector<std::byte> MemoryReader::readData(const std::filesystem::path& path) {
    return m_storage->at(path);
}

MemoryWriter::MemoryWriter(std::shared_ptr<MemoryStorage> storage)
    : m_storage{std::move(storage)} {
}

bool MemoryWriter::writeData(const std::filesystem::path& path,
                             const std::vector<std::byte>& data) {
    (*m_storage)[path] = data;
    return true;
}

std::unique_ptr<IReader> FileIOFactory::createReader() const {
    return std::make_unique<FileReader>();
}

std::unique_ptr<IWriter> FileIOFactory::createWriter() const {
    return std::make_unique<FileWriter>();
}

MemoryIOFactory::MemoryIOFactory()
    : m_storage{std::make_shared<MemoryStorage>()} {
}

std::unique_ptr<IReader> MemoryIOFactory::createReader() const {
    return std::make_unique<MemoryReader>(m_storage);
}

std::unique_ptr<IWriter> MemoryIOFactory::createWriter() const {
    return std::make_unique<MemoryWriter>(m_storage);
}
