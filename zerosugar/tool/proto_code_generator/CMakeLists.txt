add_executable (zerosugar_proto_code_generator)

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(zerosugar_proto_code_generator PUBLIC
    protobuf::libprotoc
    protobuf::libprotobuf
    Boost::boost
    nlohmann_json
    nlohmann_json::nlohmann_json
)

file(GLOB_RECURSE ZEROSUGAR_PROTO_CODE_GENERATOR_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_proto_code_generator PRIVATE ${ZEROSUGAR_PROTO_CODE_GENERATOR_SRC_FILES})
