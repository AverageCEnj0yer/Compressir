#include "app/Application.h"
#include "app/CompressionRequestBuilder.h"
#include "archive/Archiver.h"
#include "archive/CompressionService.h"
#include "io/IOFactory.h"
#include "patterns/FileItem.h"
#include "patterns/FolderItem.h"
#include "patterns/SafeWriterProxy.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace {

void writeTextFile(const std::filesystem::path& path, const std::string& content) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(content.data(), static_cast<std::streamsize>(content.size()));
}

std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

int runDemo() {
    namespace fs = std::filesystem;
    const fs::path demoRoot = fs::temp_directory_path() / "compressir_demo";
    fs::remove_all(demoRoot);

    const fs::path docsDir = demoRoot / "docs";
    const fs::path notesPath = docsDir / "notes.txt";
    const fs::path repeatPath = docsDir / "nested" / "pattern.txt";
    const fs::path topPath = demoRoot / "readme.txt";

    std::string notesText;
    for (int i = 0; i < 40; ++i) {
        notesText += "Design patterns make software modular, extensible, and easier to maintain.\n";
    }
    std::string repeatText(1200, 'Z');
    repeatText += std::string(800, 'A');
    const std::string topText = "Compressir demo archive root file.\n";

    writeTextFile(notesPath, notesText);
    writeTextFile(repeatPath, repeatText);
    writeTextFile(topPath, topText);

    const std::uint64_t rawTotal =
        fs::file_size(notesPath) + fs::file_size(repeatPath) + fs::file_size(topPath);

    std::cout << "=== Compressir Structural Patterns Demo ===\n";
    std::cout << "1) Composite (IArchiveItem: FileItem + FolderItem)\n";

    // Build a Composite tree explicitly mixing a single FileItem and nested FolderItems
    auto rootComposite = std::make_unique<FolderItem>("");
    rootComposite->add(std::make_unique<FileItem>(topPath));
    rootComposite->add(buildArchiveItemFromPath(docsDir));

    FileIOFactory ioFactory;
    auto reader = ioFactory.createReader();
    std::cout << "   Total input size via IArchiveItem::totalSize(): "
              << rootComposite->totalSize(*reader) << " B\n\n";

    std::cout << "2) Decorator (ChecksumDecorator + XorCipherDecorator around Factory Method compressors)\n";
    const fs::path archivePath = demoRoot / "bundle.cmpr";
    auto compressRequest = CompressionRequestBuilder{}
        .operation(Operation::Compress)
        .output(archivePath)
        .automaticAlgorithmSelection()
        .checksum(true)
        .encryption(true, 0x5A)
        .build();

    {
        auto writer = std::make_unique<SafeWriterProxy>(ioFactory.createWriter(), demoRoot);
        CompressionService service{
            std::make_unique<Archiver>(),
            std::move(writer),
            ioFactory.createReader()
        };
        service.compress(*rootComposite, compressRequest);
    }

    const std::uint64_t packedSize = fs::file_size(archivePath);
    std::cout << "   Size reduction: " << rawTotal << " B -> " << packedSize << " B\n\n";

    std::cout << "3) Proxy (SafeWriterProxy guarding extraction output directory)\n";
    const fs::path extractDir = demoRoot / "extracted";
    {
        auto guardedWriter = std::make_unique<SafeWriterProxy>(ioFactory.createWriter(), extractDir);

        // Demonstrate blocking a path-traversal write before delegating to FileWriter:
        const std::vector<std::byte> dummy{std::byte{'x'}};
        const bool escaped = guardedWriter->writeData(extractDir / "../../evil.txt", dummy);
        std::cout << "   Path traversal write allowed? " << (escaped ? "YES" : "NO (blocked)") << "\n";

        CompressionService service{
            std::make_unique<Archiver>(),
            std::move(guardedWriter),
            ioFactory.createReader()
        };

        auto decompressRequest = CompressionRequestBuilder{}
            .operation(Operation::Decompress)
            .addFile(archivePath)
            .output(extractDir)
            .encryption(true, 0x5A)
            .build();
        service.decompress(decompressRequest);
    }

    const bool exactMatch =
        readTextFile(extractDir / "readme.txt") == topText &&
        readTextFile(extractDir / "docs" / "notes.txt") == notesText &&
        readTextFile(extractDir / "docs" / "nested" / "pattern.txt") == repeatText;

    std::cout << "\nRestored files match originals byte-for-byte: "
              << (exactMatch ? "YES" : "NO") << '\n';

    fs::remove_all(demoRoot);
    return exactMatch ? 0 : 1;
}

std::string quoteIfNeeded(const std::string& arg) {
    if (arg.find_first_of(" \t\"") == std::string::npos) {
        return arg;
    }
    std::string quoted = "\"";
    for (char ch : arg) {
        if (ch == '"' || ch == '\\') {
            quoted.push_back('\\');
        }
        quoted.push_back(ch);
    }
    quoted.push_back('"');
    return quoted;
}

void printUsage(const char* programName) {
    std::cout << "Usage:\n"
              << "  " << programName << "                     Run structural patterns demo\n"
              << "  " << programName << " compress <paths...> -o <archive.cmpr> [--algo auto|rle|huffman] [--checksum] [--encrypt]\n"
              << "  " << programName << " decompress <archive.cmpr> -o <output_dir>\n";
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc <= 1) {
            return runDemo();
        }

        const std::string firstArg = argv[1];
        if (firstArg == "--help" || firstArg == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        if (firstArg == "demo") {
            return runDemo();
        }

        std::string command;
        for (int i = 1; i < argc; ++i) {
            if (i > 1) {
                command.push_back(' ');
            }
            command += quoteIfNeeded(argv[i]);
        }

        Application app{command};
        app.run();
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
