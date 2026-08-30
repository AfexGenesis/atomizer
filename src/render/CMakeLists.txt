set(RENDER_SOURCES vulkan.cpp camera.cpp)
set(RENDER_HEADERS vulkan.hpp camera.hpp)

add_library(atomizer_render STATIC ${RENDER_SOURCES} ${RENDER_HEADERS})
target_link_libraries(atomizer_render PUBLIC Vulkan::Vulkan Qt6::Gui)