add_executable(zerosugar_xr_application_service_coordination)

target_link_libraries(zerosugar_xr_application_service_coordination PRIVATE
    zerosugar_xr_pch
    zerosugar_xr_service_coordination
)

target_precompile_headers(zerosugar_xr_application_service_coordination REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_APPLICATION_SERVICE_COORDINATION_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_application_service_coordination PRIVATE ${ZEROSUGAR_XR_APPLICATION_SERVICE_COORDINATION_SRC_FILES})
