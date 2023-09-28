add_executable(zerosugar_test_core)

target_link_libraries(zerosugar_test_core PRIVATE
    zerosugar_core
	GTest::gtest
	GTest::gtest_main

)

target_precompile_headers(zerosugar_test_core PRIVATE
	<gtest/gtest.h>
)

file(GLOB_RECURSE ZEROSUGAR_CORE_TEST_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_test_core PRIVATE ${ZEROSUGAR_CORE_TEST_SRC_FILES})
