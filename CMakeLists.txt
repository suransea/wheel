cmake_minimum_required(VERSION 3.16)
project(wheel)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(src SOURCE)

add_library(wheel ${SOURCE})

include_directories(
        include
)

# TEST
add_subdirectory(test)
