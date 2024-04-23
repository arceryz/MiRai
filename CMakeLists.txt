cmake_minimum_required(VERSION 3.5.0)
project(Mirai VERSION 2.0.0 LANGUAGES CXX)

include(FetchContent)
set(RAYLIB_VERSION 5.0)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS ${RAYLIB_VERSION}
)
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(OPENGL_VERSION "4.3")
FetchContent_MakeAvailable(raylib)

include_directories(Source/Ray2)
include_directories(Source)
file(GLOB SOURCES Source/*.cpp Source/Ray2/*.cpp)

add_executable(Mirai ${SOURCES})
target_link_libraries(Mirai raylib)