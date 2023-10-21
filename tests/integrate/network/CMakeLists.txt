add_executable(zerosugar_test_integrate_network)

target_link_libraries(zerosugar_test_integrate_network PRIVATE
    zerosugar_shared
	GTest::gtest
	GTest::gtest_main

)

target_precompile_headers(zerosugar_test_integrate_network PRIVATE
	<gtest/gtest.h>
)

file(GLOB_RECURSE ZEROSUGAR_INTEGRATE_NETWORK_TEST_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_test_integrate_network PRIVATE ${ZEROSUGAR_INTEGRATE_NETWORK_TEST_SRC_FILES})
