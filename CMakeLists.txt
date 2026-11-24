cmake_minimum_required(VERSION 3.20)

project(Compressir LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(configure_target target)
    target_include_directories(${target} PUBLIC include)

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

add_library(compressir_lib STATIC
    src/app/Application.cpp
    src/app/CompressionRequestBuilder.cpp
    src/archive/Archiver.cpp
    src/archive/CompressionService.cpp
    src/compression/AlgorithmCreator.cpp
    src/compression/HuffmanCompressor.cpp
    src/compression/RLECompressor.cpp
    src/io/FileReader.cpp
    src/io/FileWriter.cpp
    src/io/IOFactory.cpp
    src/patterns/AlgorithmDecorator.cpp
    src/patterns/ChecksumDecorator.cpp
    src/patterns/FileItem.cpp
    src/patterns/FolderItem.cpp
    src/patterns/SafeWriterProxy.cpp
    src/patterns/XorCipherDecorator.cpp
)
configure_target(compressir_lib)

add_executable(main src/main.cpp)
configure_target(main)
target_link_libraries(main PRIVATE compressir_lib)

enable_testing()
add_executable(compressir_tests tests/test_main.cpp)
configure_target(compressir_tests)
target_link_libraries(compressir_tests PRIVATE compressir_lib)
add_test(NAME compressir_tests COMMAND compressir_tests)
