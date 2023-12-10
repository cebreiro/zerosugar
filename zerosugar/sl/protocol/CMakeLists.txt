add_library(zerosugar_sl_protocol)

target_link_libraries(zerosugar_sl_protocol PUBLIC
    zerosugar_shared
)

target_precompile_headers(zerosugar_sl_protocol REUSE_FROM zerosugar_sl_pch)

file(GLOB_RECURSE ZEROSUGAR_SL_PROTOCOL_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_sl_protocol PRIVATE ${ZEROSUGAR_SL_PROTOCOL_SRC_FILES})

set(ZEROSUGAR_SL_PROTOCOL_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(ZEROSUGAR_SL_PROTOCOL_SCHEMA ${CMAKE_CURRENT_SOURCE_DIR}/packet/schema)
set(ZEROSUGAR_SL_PROTOCOL_OUT ${CMAKE_CURRENT_SOURCE_DIR}/packet/generated)
