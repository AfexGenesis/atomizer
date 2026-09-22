set(ATOM_SOURCES atom.cpp linkage.cpp atomselect.cpp)

add_library(atomizer_atoms STATIC ${ATOM_SOURCES})
target_link_libraries(atomizer_atoms PUBLIC atomizer_core atomizer_atom atomizer_molecule)