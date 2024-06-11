add_library(zerosugar_xr_network)

target_link_libraries(zerosugar_xr_network PRIVATE
    zerosugar_xr_pch
)

target_precompile_headers(zerosugar_xr_network REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_NETWORK_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_network PRIVATE ${ZEROSUGAR_XR_NETWORK_SRC_FILES})


set(ZEROSUGAR_XR_NETWORK_MODEL_DIR ${PROJECT_SOURCE_DIR}/zerosugar/xr/network/model/)

add_custom_target(zerosugar_xr_generate_netcode
    COMMAND ${ZEROSUGAR_PROTOBUF_PATH}
        --proto_path=${ZEROSUGAR_PROTOBUF_INCLUDE}
        --proto_path=./
        --plugin=protoc-gen-zerosugar_proto_code_generator=$<TARGET_FILE_DIR:zerosugar_proto_code_generator>/zerosugar_proto_code_generator.exe
        --zerosugar_proto_code_generator_opt="{ \"\"include_path\"\" : \"\"zerosugar/xr/network/generated\"\",\"\"generator\"\" : \"\"xr_packet_code_generator\"\" }"
        --zerosugar_proto_code_generator_out=./generated
        service_to_service.proto
        login_cs.proto
        login_sc.proto
    WORKING_DIRECTORY ${ZEROSUGAR_XR_NETWORK_MODEL_DIR}
    DEPENDS zerosugar_proto_code_generator
)
