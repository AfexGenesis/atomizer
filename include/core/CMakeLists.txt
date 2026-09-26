set(CORE_HEADERS cif.hpp grid.hpp parser.hpp)

add_subdirectory(atom)
add_subdirectory(molecule)
add_library(atomizer_core STATIC ${CORE_HEADERS})
target_include_directories(atomizer_core PUBLIC .)