add_executable (zerosugar_sl_proto_code_generator)

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(zerosugar_sl_proto_code_generator PUBLIC
    protobuf::libprotoc
    protobuf::libprotobuf
    Boost::boost
    nlohmann_json
    nlohmann_json::nlohmann_json
)

file(GLOB_RECURSE ZEROSUGAR_SL_PROTO_CODE_GENERATOR_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_sl_proto_code_generator PRIVATE ${ZEROSUGAR_SL_PROTO_CODE_GENERATOR_SRC_FILES})

set(ZEROSUGAR_GENERATE_SERVICE_MODEL_DIR ${PROJECT_SOURCE_DIR}/zerosugar/sl/service/model/)

add_custom_target(zerosugar_sl_generate_service_code
    COMMAND ${ZEROSUGAR_PROTOBUF_PATH}
        --proto_path=${ZEROSUGAR_PROTOBUF_INCLUDE}
        --proto_path=./
        --plugin=protoc-gen-zerosugar_sl_proto_code_generator=$<TARGET_FILE_DIR:zerosugar_sl_proto_code_generator>/zerosugar_sl_proto_code_generator.exe
        --zerosugar_sl_proto_code_generator_opt="{ \"\"include_path\"\" : \"\"zerosugar/sl/service/generated\"\", \"\"generator\"\" : \"\"service_code_generator\"\" }"
        --zerosugar_sl_proto_code_generator_out=../generated
        shared.proto
        config.proto
        login_service.proto
        repository_service.proto
        world_service.proto
    WORKING_DIRECTORY ${ZEROSUGAR_GENERATE_SERVICE_MODEL_DIR}
    DEPENDS zerosugar_sl_proto_code_generator
)

set(ZEROSUGAR_GENERATE_PACKET_MODEL_DIR ${PROJECT_SOURCE_DIR}/zerosugar/sl/protocol/packet/model/)

add_custom_target(zerosugar_sl_generate_sl_packet_code
    COMMAND ${ZEROSUGAR_PROTOBUF_PATH}
        --proto_path=${ZEROSUGAR_PROTOBUF_INCLUDE}
        --proto_path=./
        --plugin=protoc-gen-zerosugar_sl_proto_code_generator=$<TARGET_FILE_DIR:zerosugar_sl_proto_code_generator>/zerosugar_sl_proto_code_generator.exe
        --zerosugar_sl_proto_code_generator_opt="{ \"\"include_path\"\" : \"\"zerosugar/sl/protocol/packet/generated\"\", \"\"generator\"\" : \"\"sl_packet_code_generator\"\" }"
        --zerosugar_sl_proto_code_generator_out=../generated
        gateway_object.proto
        gateway_sc.proto
        gateway_cs.proto
    WORKING_DIRECTORY ${ZEROSUGAR_GENERATE_PACKET_MODEL_DIR}
    DEPENDS zerosugar_sl_proto_code_generator
)
