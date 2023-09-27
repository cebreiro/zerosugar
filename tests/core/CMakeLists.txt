add_executable(zerosugar_test_core)

target_link_libraries(zerosugar_test_core PRIVATE
    zerosugar_core
	GTest::gtest
	GTest::gtest_main

)

target_sources(zerosugar_test_core PRIVATE
	test_core.cpp
)

target_precompile_headers(zerosugar_test_core PRIVATE
	<gtest/gtest.h>
)

add_subdirectory(execution)
