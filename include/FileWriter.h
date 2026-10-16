#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "IWriter.h"
#include <string>
#include <iostream>

//reading files from a directory
class FileWriter : public IWriter
{
    private:
    std::string directory{};

    public:
    bool writeData() override 
    {
        //placeholder
        std::cout << "Writing data\n";
        return true;
    }
};

#endif
