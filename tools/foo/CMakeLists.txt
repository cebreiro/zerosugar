cmake_minimum_required (VERSION 3.8)

project(test_libprotobuf)

add_executable (test_libprotobuf 
	test_libprotobuf.cpp
)

#add_custom_target




target_link_libraries(test_libprotobuf PUBLIC 
	protobuf::libprotoc
	protobuf::libprotobuf
	GTest::gtest
	GTest::gtest_main
    Boost::boost
)

message("CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")
