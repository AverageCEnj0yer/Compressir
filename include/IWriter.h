#ifndef IWRITER_H
#define IWRITER_H

#include <string_view>

class IWriter
{
    public:
    virtual bool writeData() = 0;

    virtual ~IWriter() = default;
};

#endif
