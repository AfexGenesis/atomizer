set(RENDER_SOURCES vulkan.cpp camera.cpp window.cpp)

add_subdirectory(model)
add_subdirectory(atoms)
add_library(atomizer_render STATIC ${RENDER_SOURCES})
target_link_libraries(atomizer_render PUBLIC atomizer_model atomizer_atoms Vulkan::Vulkan Qt6::Gui atomizer_atoms atomizer_model atomizer_core atomizer_atom atomizer_molecule)