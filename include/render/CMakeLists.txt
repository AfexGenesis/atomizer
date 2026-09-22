set(RENDER_HEADERS vulkan.hpp camera.hpp window.hpp)

add_subdirectory(model)
add_subdirectory(atoms)
add_library(atomizer_render STATIC ${RENDER_HEADERS})
target_link_libraries(atomizer_render PUBLIC Vulkan::Vulkan Qt6::Gui atomizer_atoms atomizer_model atomizer_core atomizer_atom atomizer_molecule)