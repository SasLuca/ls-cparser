cmake_minimum_required(VERSION 3.15)
project(ls-cparser C)

set(CMAKE_C_STANDARD 99)

# Sanitizers
if(LS_CPARSER_SANITIZERS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
endif()

include_directories(include)
add_library(ls-cparser source/lscparser.c source/stb_ds.h include/lscparser.h)
target_include_directories(ls-cparser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

# For testing, set `-DLS_CPARSER_TESTS=on` for CMake. In CLion you can set this under `Settings > Build, Execution, Deployment > CMake > CMake Options`
if(LS_CPARSER_TESTS)
    add_subdirectory(tests)
endif()