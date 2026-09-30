set(VERT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/vertex.hlsl)
set(FRAG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/fragment.hlsl)

set(VERT_SPV ${CMAKE_CURRENT_BINARY_DIR}/vertex.spv)
set(FRAG_SPV ${CMAKE_CURRENT_BINARY_DIR}/fragment.spv)
set(MODEL_VERT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/modelvertex.hlsl)
set(MODEL_FRAG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/modelfragment.hlsl)
set(MODEL_VERT_SPV ${CMAKE_CURRENT_BINARY_DIR}/modelvertex.spv)
set(MODEL_FRAG_SPV ${CMAKE_CURRENT_BINARY_DIR}/modelfragment.spv)
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

add_custom_command(
    OUTPUT ${MODEL_VERT_SPV}
    COMMAND ${DXC_EXECUTABLE} -spirv -T vs_6_0 -E main ${MODEL_VERT_SRC} -Fo ${MODEL_VERT_SPV}
    DEPENDS ${MODEL_VERT_SRC}
)
add_custom_command(
    OUTPUT ${MODEL_FRAG_SPV}
    COMMAND ${DXC_EXECUTABLE} -spirv -T ps_6_0 -E main ${MODEL_FRAG_SRC} -Fo ${MODEL_FRAG_SPV}
    DEPENDS ${MODEL_FRAG_SRC}
)
add_custom_target(atomizer_shaders
    DEPENDS
        ${VERT_SPV}
        ${FRAG_SPV}
        ${MODEL_VERT_SPV}
        ${MODEL_FRAG_SPV}
)

install(FILES
    ${VERT_SPV}
    ${FRAG_SPV}
    ${MODEL_VERT_SPV}
    ${MODEL_FRAG_SPV}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/atomizer/shaders
)