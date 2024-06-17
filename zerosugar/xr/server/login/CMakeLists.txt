add_library(zerosugar_xr_server_login)

target_link_libraries(zerosugar_xr_server_login PRIVATE
    zerosugar_xr_pch
    zerosugar_xr_network
)

target_precompile_headers(zerosugar_xr_server_login REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_SERVER_LOGIN_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_server_login PRIVATE ${ZEROSUGAR_XR_SERVER_LOGIN_SRC_FILES})
