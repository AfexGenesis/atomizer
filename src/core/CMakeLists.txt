set(CORE_SOURCES memory.cpp pipeline.cpp cif.cpp grid.cpp parser.cpp)

add_subdirectory(atom)
add_subdirectory(molecule)
add_library(atomizer_core STATIC ${CORE_SOURCES})
target_include_directories(atomizer_core PUBLIC .)