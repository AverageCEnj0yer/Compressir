#ifndef APPLICATION_H
#define APPLICATION_H

#include "CompressionRequest.h"
#include "CompressionService.h"
#include "IOFactory.h"
#include <string>
#include <string_view>



class Application {
private:
    std::string m_command{};

    CompressionService createService(
        const IIOFactory& ioFactory
    );

    void handleCompress(const CompressionRequest& request);

    void handleDecompress(const CompressionRequest& request);

public:
    explicit Application(std::string_view command);

    CompressionRequest buildRequest();

    void run();
};

#endif
