add_executable (zerosugar_sl_sox_code_generator)

target_link_libraries(zerosugar_sl_sox_code_generator PUBLIC
    Boost::boost
)

file(GLOB_RECURSE ZEROSUGAR_SL_SOX_CODE_GENERATOR_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_sl_sox_code_generator PRIVATE ${ZEROSUGAR_SL_SOX_CODE_GENERATOR_SRC_FILES})
