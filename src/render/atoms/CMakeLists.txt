set(ATOM_SOURCES atom.cpp)
set(ATOM_HEADERS atom.hpp)

add_library(atomizer_atoms STATIC ${ATOM_SOURCES} ${ATOM_HEADERS})
target_link_libraries(atomizer_atoms)