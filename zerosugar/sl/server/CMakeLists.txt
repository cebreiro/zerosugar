add_library(zerosugar_sl_server)

target_link_libraries(zerosugar_sl_server PRIVATE
    zerosugar_sl_pch
    zerosugar_sl_protocol
    zerosugar_sl_service
)

target_precompile_headers(zerosugar_sl_server REUSE_FROM zerosugar_sl_pch)

file(GLOB_RECURSE ZEROSUGAR_SL_SERVER_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_sl_server PRIVATE ${ZEROSUGAR_SL_SERVER_SRC_FILES})
