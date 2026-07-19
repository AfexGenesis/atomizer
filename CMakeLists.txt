cmake_minimum_required(VERSION 4.3)
project(
    atomizer
    VERSION 0.1
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Vulkan)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
add_subdirectory(src)