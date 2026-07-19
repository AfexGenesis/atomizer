set(RENDER_SOURCES vulkan.cpp)
set(RENDER_HEADERS vulkan.hpp)

add_library(atomizer_render STATIC ${RENDER_SOURCES} ${RENDER_HEADERS})
target_link_libraries(atomizer_render PUBLIC Vulkan::Vulkan Qt6::Gui)