#ifndef COMPRESSIONSERVICE_H
#define COMPRESSIONSERVICE_H

#include "Archiver.h"
#include "ICompressionAlgorithm.h"
#include "IReader.h"
#include "IWriter.h"
#include <memory>

class CompressionService
{
    private:
    std::unique_ptr<Archiver> m_archiver{};
    std::unique_ptr<ICompressionAlgorithm> m_compressor{};
    std::unique_ptr<IWriter> m_writer{};
    std::unique_ptr<IReader> m_reader{};

    public:
    CompressionService( std::unique_ptr<Archiver> archiver,
                        std::unique_ptr<ICompressionAlgorithm> compressor,
                        std::unique_ptr<IWriter> writer,
                        std::unique_ptr<IReader> reader) : 
    
};

#endif
