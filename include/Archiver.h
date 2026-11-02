#ifndef ARCHIVER_H
#define ARCHIVER_H

#include "ArchiveEntry.h"

#include <vector>
#include <cstddef>

class Archiver {
public:
    std::vector<std::byte> pack(
        const std::vector<ArchiveEntry>& entries
    );

    std::vector<ArchiveEntry> unpack(
        [[maybe_unused]]const std::vector<std::byte>& archiveData
    );
};

#endif
