set(VERT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/vertex.hlsl)
set(FRAG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/fragment.hlsl)

set(VERT_SPV ${CMAKE_CURRENT_BINARY_DIR}/vertex.spv)
set(FRAG_SPV ${CMAKE_CURRENT_BINARY_DIR}/fragment.spv)
find_program(DXC_EXECUTABLE dxc REQUIRED)
add_custom_command(
    OUTPUT ${VERT_SPV}
    COMMAND ${DXC_EXECUTABLE} -spirv -T vs_6_0 -E main ${VERT_SRC} -Fo ${VERT_SPV}
    DEPENDS ${VERT_SRC}
)

add_custom_command(
    OUTPUT ${FRAG_SPV}
    COMMAND ${DXC_EXECUTABLE} -spirv -T ps_6_0 -E main ${FRAG_SRC} -Fo ${FRAG_SPV}
    DEPENDS ${FRAG_SRC}
)

add_library(atomizer_shaders STATIC)
qt_add_resources(atomizer_shaders "shader_resources"
    PREFIX "/shaders"
    BASE ${CMAKE_CURRENT_BINARY_DIR}
    FILES
        ${VERT_SPV}
        ${FRAG_SPV}
)

target_link_libraries(atomizer_shaders PUBLIC Qt6::Core)