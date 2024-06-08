add_library(zerosugar_xr_service_zone)

target_link_libraries(zerosugar_xr_service_zone PRIVATE
    zerosugar_xr_pch
)

target_precompile_headers(zerosugar_xr_service_zone REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_SERVICE_ZONE_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_service_zone PRIVATE ${ZEROSUGAR_XR_SERVICE_ZONE_SRC_FILES})
