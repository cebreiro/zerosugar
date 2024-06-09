set(ZEROSUGAR_XR_SERVICE_DIR ${PROJECT_SOURCE_DIR}/zerosugar/xr/service/model/)

add_custom_target(zerosugar_xr_generate_service
    COMMAND ${ZEROSUGAR_PROTOBUF_PATH}
        --proto_path=${ZEROSUGAR_PROTOBUF_INCLUDE}
        --proto_path=./
        --plugin=protoc-gen-zerosugar_proto_code_generator=$<TARGET_FILE_DIR:zerosugar_proto_code_generator>/zerosugar_proto_code_generator.exe
        --zerosugar_proto_code_generator_opt="{ \"\"include_path\"\" : \"\"zerosugar/xr/service/model/generated\"\", \"\"generator\"\" : \"\"service_code_generator\"\" }"
        --zerosugar_proto_code_generator_out=./generated
        login_service.proto
        orchestrator_service.proto
    WORKING_DIRECTORY ${ZEROSUGAR_XR_SERVICE_DIR}
    DEPENDS zerosugar_proto_code_generator
)
