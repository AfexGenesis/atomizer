set(MODEL_SOURCES model.cpp modelselect.cpp)

add_library(atomizer_model STATIC ${MODEL_SOURCES})
target_include_directories(atomizer_model PUBLIC .)
target_link_libraries(atomizer_model PUBLIC atomizer_core atomizer_molecule)