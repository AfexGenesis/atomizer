set(ATOM_HEADERS atom.hpp atomselect.hpp)

add_library(atomizer_atoms STATIC ${ATOM_HEADERS})
target_link_libraries(atomizer_atoms PUBLIC atomizer_core atomizer_atom atomizer_molecule)