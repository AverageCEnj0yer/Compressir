#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <initializer_list>
#include <string_view>
#include <vector>
#include <cstddef>
#include <iostream>

class Archiver
{
    private:
    using File = std::vector<std::byte>;
    
    std::vector<File> m_files{};

    public:
    Archiver() = default;
    Archiver(std::initializer_list<File> files) : m_files{files} {};

    void add(File file)
    {
        m_files.push_back(file);
    }

    void archive(std::string_view archiveName)
    {
        //archiver logic to be added here.
        std::cout << "Archived " << m_files.size() << " files in " << archiveName;
    }
};

#endif
