set(ATOM_SOURCES atominfo.cpp element.cpp)
set(ATOM_HEADERS atominfo.hpp element.hpp)

add_library(atomizer_atom STATIC ${ATOM_SOURCES} ${ATOM_HEADERS})
target_include_directories(atomizer_atom PUBLIC .)
target_link_libraries(atomizer_atom PUBLIC atomizer_core)