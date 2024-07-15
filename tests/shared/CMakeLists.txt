add_executable(zerosugar_test_shared)

target_link_libraries(zerosugar_test_shared PRIVATE
    zerosugar_shared
	GTest::gtest
	GTest::gtest_main
)

target_precompile_headers(zerosugar_test_shared PRIVATE
	<gtest/gtest.h>
)

file(GLOB_RECURSE ZEROSUGAR_SHARED_TEST_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_test_shared PRIVATE ${ZEROSUGAR_SHARED_TEST_SRC_FILES})
