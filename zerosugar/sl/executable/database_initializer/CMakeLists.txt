add_executable(zerosugar_sl_database_initializer)

target_link_libraries(zerosugar_sl_database_initializer PRIVATE
    zerosugar_sl_pch
    zerosugar_sl_database
)

target_precompile_headers(zerosugar_sl_database_initializer REUSE_FROM zerosugar_sl_pch)

file(GLOB_RECURSE ZEROSUGAR_SL_DATABASE_INITIALIZER_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_sl_database_initializer PRIVATE ${ZEROSUGAR_SL_DATABASE_INITIALIZER_SRC_FILES})
