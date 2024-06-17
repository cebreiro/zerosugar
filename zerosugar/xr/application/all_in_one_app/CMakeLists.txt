add_executable(zerosugar_xr_application_all_in_one_app)

target_link_libraries(zerosugar_xr_application_all_in_one_app PRIVATE
    zerosugar_xr_pch
    zerosugar_xr_network

    zerosugar_xr_server_login
    zerosugar_xr_server_lobby

    zerosugar_xr_service_login
    zerosugar_xr_service_gateway
    zerosugar_xr_service_game
    zerosugar_xr_service_database
)

target_precompile_headers(zerosugar_xr_application_all_in_one_app REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_APPLICATION_ALL_IN_ONE_APP_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_application_all_in_one_app PRIVATE ${ZEROSUGAR_XR_APPLICATION_ALL_IN_ONE_APP_SRC_FILES})
