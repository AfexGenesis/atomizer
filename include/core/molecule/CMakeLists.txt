set(MOLECULE_SOURCES linkage.cpp module.cpp topology.cpp template.cpp)
set(MOLECULE_HEADERS linkage.hpp module.hpp topology.hpp template.hpp)

add_library(atomizer_molecule STATIC ${MOLECULE_SOURCES} ${MOLECULE_HEADERS})
target_include_directories(atomizer_molecule PUBLIC .)
target_link_libraries(atomizer_molecule PUBLIC atomizer_core atomizer_atom)