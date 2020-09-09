enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)

# Sanitizers
if(LS_CPARSER_SANITIZERS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

add_executable(tests catch2-main.cpp tests.cpp)
target_link_libraries(tests PRIVATE lscp)
target_include_directories(tests PRIVATE ../source)

add_executable(playground playground.c)
target_link_libraries(playground PRIVATE lscp)