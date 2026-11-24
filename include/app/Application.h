#ifndef APPLICATION_H
#define APPLICATION_H

#include "archive/CompressionService.h"
#include "io/IOFactory.h"
#include "models/CompressionRequest.h"

#include <filesystem>
#include <string>
#include <string_view>

class Application {
private:
    std::string m_command{};

    CompressionService createService(
        const IIOFactory& ioFactory,
        const std::filesystem::path& allowedWriteDir
    );

    void handleCompress(const CompressionRequest& request);
    void handleDecompress(const CompressionRequest& request);

public:
    explicit Application(std::string_view command);

    CompressionRequest buildRequest();

    void run();
};

#endif
