#ifndef IREADER_H
#define IREADER_H

#include <string_view>

class IReader
{
    public:
    virtual bool readData() = 0;

    virtual ~IReader() = default;
};

#endif
