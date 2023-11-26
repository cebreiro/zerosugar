add_executable(zerosugar_sl_database)

find_package(OpenSSL REQUIRED)

target_link_libraries(zerosugar_sl_database PRIVATE
    OpenSSL::Crypto
	OpenSSL::SSL
    zerosugar_shared
)

file(GLOB_RECURSE ZEROSUGAR_SL_DATABASE_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_sl_database PRIVATE ${ZEROSUGAR_SL_DATABASE_SRC_FILES})


set(ZEROSUGAR_DB_GENETOR_TOOL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../tool/dotnet/DatabaseCodeGenetor/bin/Release/net8.0/DatabaseCodeGenetor.dll)
set(ZEROSUGAR_DB_TARGET_PATH ${CMAKE_CURRENT_SOURCE_DIR}/model/)
set(ZEROSUGAR_DB_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/generated/)

add_custom_target(zerosugar_generate_database_code
    COMMAND dotnet ${ZEROSUGAR_DB_GENETOR_TOOL_PATH}
        ${ZEROSUGAR_DB_TARGET_PATH}
        ${ZEROSUGAR_DB_OUTPUT_PATH}
        --include-directory=${CMAKE_CURRENT_SOURCE_DIR}/../../../
        -v
)
