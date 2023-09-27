add_library(zerosugar_core)

target_link_libraries(zerosugar_core PUBLIC
	Boost::boost
	TBB::tbb
	spdlog::spdlog
)

add_subdirectory(execution)

file(GLOB_RECURSE ZEROSUGAR_CORE_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_core PRIVATE ${ZEROSUGAR_CORE_SRC_FILES})
