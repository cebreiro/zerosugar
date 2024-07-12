add_executable(zerosugar_xr_application_bot_client)

target_link_libraries(zerosugar_xr_application_bot_client PRIVATE
    zerosugar_xr_pch
    zerosugar_xr_network
    zerosugar_xr_navigation
    zerosugar_xr_data
)

target_precompile_headers(zerosugar_xr_application_bot_client REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_APPLICATION_BOT_CLIENT_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_application_bot_client PRIVATE ${ZEROSUGAR_XR_APPLICATION_BOT_CLIENT_SRC_FILES})
