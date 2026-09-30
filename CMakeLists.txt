cmake_minimum_required(VERSION 4.3)
project(
    atomizer
    VERSION 0.1
    LANGUAGES CXX
)
include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Vulkan REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(src)
add_subdirectory(shaders)
add_dependencies(atomizer atomizer_shaders)
qt_add_resources(atomizer "atomizer_resources" PREFIX "/" FILES 
resources/fonts/orbitron-regular.ttf
resources/fonts/inter-24pt-regular.ttf
resources/assets/atomizer.svg)