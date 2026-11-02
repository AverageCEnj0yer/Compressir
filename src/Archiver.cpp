#include "Archiver.h"

#include <iostream>

std::vector<std::byte> Archiver::pack(
        const std::vector<ArchiveEntry>& entries
    ) {
        // TODO: Serialize archive header.
        // TODO: Serialize each entry's metadata.
        // TODO: Append compressed file contents.
        // TODO: Return complete archive bytes.

        std::cout << "Packing "
                  << entries.size()
                  << " files into an archive...\n";

        return {};
    }

std::vector<ArchiveEntry> Archiver::unpack(
        [[maybe_unused]]const std::vector<std::byte>& archiveData
    ) {
        // TODO: Parse archive header.
        // TODO: Read individual archive entries.
        // TODO: Recover filenames and metadata.
        // TODO: Return compressed entries.
        // TODO: Reject corrupted archive structures.

        std::cout << "Unpacking archive structure...\n";

        return {};
    }
