cmake_minimum_required(VERSION 3.16)
project(wheel)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    include
)

add_subdirectory(tests)

enable_testing()
