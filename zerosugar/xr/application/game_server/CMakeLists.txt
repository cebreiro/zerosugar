add_executable(zerosugar_xr_application_server_game)

target_link_libraries(zerosugar_xr_application_server_game PRIVATE
    zerosugar_xr_pch
    zerosugar_xr_server_game
)

target_precompile_headers(zerosugar_xr_application_server_game REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_APPLICATION_SERVER_GAME_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_application_server_game PRIVATE ${ZEROSUGAR_XR_APPLICATION_SERVER_GAME_SRC_FILES})
