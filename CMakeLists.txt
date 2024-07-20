cmake_minimum_required(VERSION 3.28)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

#Add SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})

add_executable(untitled main.cpp
        main.cpp
        SDLWindow.h
        SDLWindow.cpp
        Vector.h
        MapManager.cpp
        MapManager.h)

# Link SDL2
target_link_libraries(untitled ${SDL2_LIBRARIES})
