set(ATOM_HEADERS atominfo.hpp element.hpp)

add_library(atomizer_atom STATIC ${ATOM_HEADERS})
target_include_directories(atomizer_atom PUBLIC .)
target_link_libraries(atomizer_atom PUBLIC atomizer_core)