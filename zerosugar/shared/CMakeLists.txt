add_library(zerosugar_shared)

target_link_libraries(zerosugar_shared PUBLIC
	Boost::boost
	TBB::tbb
	spdlog::spdlog
)

file(GLOB_RECURSE ZEROSUGAR_SHARED_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_shared PRIVATE ${ZEROSUGAR_SHARED_SRC_FILES})
