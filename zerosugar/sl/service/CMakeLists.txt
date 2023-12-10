add_library(zerosugar_sl_service)

target_link_libraries(zerosugar_sl_service PRIVATE
    zerosugar_sl_pch
    zerosugar_sl_database
)

target_precompile_headers(zerosugar_sl_service REUSE_FROM zerosugar_sl_pch)

file(GLOB_RECURSE ZEROSUGAR_SL_SERVICE_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_sl_service PRIVATE ${ZEROSUGAR_SL_SERVICE_SRC_FILES})
