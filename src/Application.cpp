#include <Application.h>

#include "CompressionRequestBuilder.h"
#include "Archiver.h"

#include <memory>
#include <iostream>


Application::Application(std::string_view command)
        : m_command{command}
    {
    }


CompressionService Application::createService(
        const IIOFactory& ioFactory
    ) {
        return CompressionService{
            std::make_unique<Archiver>(),
            ioFactory.createWriter(),
            ioFactory.createReader()
        };
    }

void Application::handleCompress(const CompressionRequest& request) {
    FileIOFactory ioFactory;
    auto service = createService(ioFactory);
    service.compress(request);
}

void Application::handleDecompress(const CompressionRequest& request) {
    FileIOFactory ioFactory;
    auto service = createService(ioFactory);
    service.decompress(request);
}

CompressionRequest Application::buildRequest() {
    // TODO: Parse m_command.
    // TODO: Extract operation, files, output path,
    //       algorithm and compression settings.
    // TODO: Validate request syntax and options.
    // TODO: Report invalid arguments.

    std::cout << "Building request...\n";

    return CompressionRequestBuilder{}.build(); // Placeholder.
}

void Application::run() {
    auto request = buildRequest();

    // TODO: Handle invalid requests before execution.

    switch (request.operation) {
        case Operation::Compress:
            handleCompress(request);
            break;

        case Operation::Decompress:
            handleDecompress(request);
            break;
    }
}
