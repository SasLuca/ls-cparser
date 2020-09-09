cmake_minimum_required(VERSION 3.1)
project(lscp C)

set(CMAKE_C_STANDARD 99)

# Sanitizers
if(LS_CPARSER_SANITIZERS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
endif()

add_library(lscp)
target_sources(lscp PRIVATE source/lscp-parser.c
                            source/lscp-allocator.c
                            source/lscp-tokenizer.c
                            source/lscp-stb.c
                            source/lscp-preprocessor.c)
target_include_directories(lscp PUBLIC include)
target_include_directories(lscp PRIVATE source)

if(LS_CPARSER_DEMO)
    add_executable(demo demo/demo.c)
    target_link_libraries(demo PRIVATE lscp)
endif()

# For testing, set `-DLS_CPARSER_TESTS=on` for CMake. In CLion you can set this under `Settings > Build, Execution, Deployment > CMake > CMake Options`
if(LS_CPARSER_TESTS)
    add_subdirectory(tests)
endif()