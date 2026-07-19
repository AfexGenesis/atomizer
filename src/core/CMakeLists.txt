set(CORE_SOURCES memory.cpp pipeline.cpp cif.cpp)
set(CORE_HEADERS cif.hpp)

add_library(atomizer_utils STATIC ${CORE_SOURCES} ${CORE_HEADERS})