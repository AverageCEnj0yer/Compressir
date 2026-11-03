#ifndef IO_FACTORY_H
#define IO_FACTORY_H

#include "IReader.h"
#include "IWriter.h"
#include <map>
#include <memory>

using MemoryStorage = std::map<std::filesystem::path, std::vector<std::byte>>;

class MemoryReader : public IReader {
private:
    std::shared_ptr<MemoryStorage> m_storage;

public:
    explicit MemoryReader(std::shared_ptr<MemoryStorage> storage);
    std::vector<std::byte> readData(const std::filesystem::path& path) override;
};

class MemoryWriter : public IWriter {
private:
    std::shared_ptr<MemoryStorage> m_storage;

public:
    explicit MemoryWriter(std::shared_ptr<MemoryStorage> storage);
    bool writeData(const std::filesystem::path& path,
                   const std::vector<std::byte>& data) override;
};

class IIOFactory {
public:
    virtual ~IIOFactory() = default;
    virtual std::unique_ptr<IReader> createReader() const = 0;
    virtual std::unique_ptr<IWriter> createWriter() const = 0;
};

class FileIOFactory : public IIOFactory {
public:
    std::unique_ptr<IReader> createReader() const override;
    std::unique_ptr<IWriter> createWriter() const override;
};

class MemoryIOFactory : public IIOFactory {
private:
    std::shared_ptr<MemoryStorage> m_storage;

public:
    MemoryIOFactory();
    std::unique_ptr<IReader> createReader() const override;
    std::unique_ptr<IWriter> createWriter() const override;
};

#endif
