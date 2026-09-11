set(CORE_SOURCES memory.cpp pipeline.cpp cif.cpp grid.cpp)
set(CORE_HEADERS cif.hpp grid.hpp)

add_library(atomizer_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(atomizer_core PUBLIC .)