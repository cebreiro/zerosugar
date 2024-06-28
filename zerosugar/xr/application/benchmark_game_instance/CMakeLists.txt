add_executable(zerosugar_xr_application_benchmark_game_instance)

target_link_libraries(zerosugar_xr_application_benchmark_game_instance PRIVATE
    zerosugar_xr_pch

    zerosugar_xr_server_game
)

target_precompile_headers(zerosugar_xr_application_benchmark_game_instance REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_APPLICATION_BENCHMARK_GAME_INSTANCE_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_application_benchmark_game_instance PRIVATE ${ZEROSUGAR_XR_APPLICATION_BENCHMARK_GAME_INSTANCE_SRC_FILES})
