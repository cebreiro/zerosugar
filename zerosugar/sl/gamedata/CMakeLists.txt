add_library(zerosugar_sl_gamedata)

target_link_libraries(zerosugar_sl_gamedata PRIVATE
    zerosugar_shared
    zerosugar_sl_pch
)

target_precompile_headers(zerosugar_sl_gamedata REUSE_FROM zerosugar_sl_pch)

file(GLOB_RECURSE ZEROSUGAR_SL_GAMEDATA_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_sl_gamedata PRIVATE ${ZEROSUGAR_SL_GAMEDATA_SRC_FILES})
