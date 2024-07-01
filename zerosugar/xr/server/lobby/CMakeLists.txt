add_library(zerosugar_xr_server_lobby)

target_link_libraries(zerosugar_xr_server_lobby PRIVATE
    zerosugar_xr_pch
    zerosugar_xr_network
    zerosugar_xr_service_model
)

target_precompile_headers(zerosugar_xr_server_lobby REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_SERVER_LOBBY_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_server_lobby PRIVATE ${ZEROSUGAR_XR_SERVER_LOBBY_SRC_FILES})
