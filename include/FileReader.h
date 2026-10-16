#ifndef FILEREADER_H
#define FILEREADER_H

#include "IReader.h"
#include <string>
#include <iostream>

//reading files from a directory
class FileReader : public IReader
{
    private:
    std::string directory{};

    public:
    bool readData() override 
    {
        //placeholder
        std::cout << "reading data\n";
        return true;
    }
};

#endif
