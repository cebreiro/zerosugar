add_library(zerosugar_xr_service_game)

target_link_libraries(zerosugar_xr_service_game PRIVATE
    zerosugar_xr_pch
)

target_precompile_headers(zerosugar_xr_service_game REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_SERVICE_GAME_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_service_game PRIVATE ${ZEROSUGAR_XR_SERVICE_GAME_SRC_FILES})
