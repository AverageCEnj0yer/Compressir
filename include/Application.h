#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include "CompressionRequest.h"
#include "CompressionService.h"

//works like application layer, gets what the user wants and redirects it accordingly
class Application
{

    private:
    std::string m_command{};


    public:
    Application(std::string_view command) : m_command{command} {};

    //this will take the command, separate arguments, call handleCompress or handleDecompress accordingly
    void run(CompressionRequest req)
    {
        //correctly construct the dependencies here (will be later replaced with a factory class)

        CompressionService serv{/*dependencies*/};
    }


    CompressionRequest buildRequest()
    {
        //extract all info from command

        //pass it to CompressionRequest

        return CompressionRequest{}; //temporary placeholder
    }


    void handleCompress();
    void handleDecompress();

};

#endif
