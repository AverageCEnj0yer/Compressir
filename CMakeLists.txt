cmake_minimum_required(VERSION 3.20)

project(Compressir LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(add_source target)

    add_executable(${target} ${ARGN})

    target_include_directories(${target} PRIVATE include)

    target_compile_options(${target} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
    )
endfunction()

add_source(main
    src/main.cpp
    src/Application.cpp
    src/Application.cpp
    src/Archiver.cpp
    src/CompressionService.cpp
    src/FileReader.cpp
    src/FileWriter.cpp
    src/HuffmanCompressor.cpp
    src/RLECompressor.cpp
)
