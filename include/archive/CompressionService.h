#ifndef COMPRESSION_SERVICE_H
#define COMPRESSION_SERVICE_H

#include "archive/Archiver.h"
#include "io/IReader.h"
#include "io/IWriter.h"
#include "models/CompressionRequest.h"
#include "patterns/IArchiveItem.h"

#include <memory>

class CompressionService {
private:
    std::unique_ptr<Archiver> m_archiver;
    std::unique_ptr<IWriter> m_writer;
    std::unique_ptr<IReader> m_reader;

public:
    CompressionService(
        std::unique_ptr<Archiver> archiver,
        std::unique_ptr<IWriter> writer,
        std::unique_ptr<IReader> reader
    );

    void compress(const CompressionRequest& request);
    void compress(const IArchiveItem& root, const CompressionRequest& request);

    void decompress(const CompressionRequest& request);
};

#endif
