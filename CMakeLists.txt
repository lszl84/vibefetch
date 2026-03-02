cmake_minimum_required(VERSION 3.16)
project(vibefetch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(vibefetch
    src/main.cpp
    src/sysinfo.cpp
    src/agents.cpp
    src/display.cpp
)
