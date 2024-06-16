add_library(zerosugar_xr_service_model)

target_link_libraries(zerosugar_xr_service_model PRIVATE
    zerosugar_xr_pch
)

target_precompile_headers(zerosugar_xr_service_model REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_SERVICE_MODEL_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_service_model PRIVATE ${ZEROSUGAR_XR_SERVICE_MODEL_SRC_FILES})

set(ZEROSUGAR_XR_SERVICE_DIR ${PROJECT_SOURCE_DIR}/zerosugar/xr/service/model/)

add_custom_target(zerosugar_xr_generate_service
    COMMAND ${ZEROSUGAR_PROTOBUF_PATH}
        --proto_path=${ZEROSUGAR_PROTOBUF_INCLUDE}
        --proto_path=./
        --plugin=protoc-gen-zerosugar_proto_code_generator=$<TARGET_FILE_DIR:zerosugar_proto_code_generator>/zerosugar_proto_code_generator.exe
        --zerosugar_proto_code_generator_opt="{ \"\"include_path\"\" : \"\"zerosugar/xr/service/model/generated\"\", \"\"generator\"\" : \"\"service_code_generator\"\" }"
        --zerosugar_proto_code_generator_out=./generated
        data_transfer_object.proto
        login_service.proto
        database_service.proto
        gateway_service.proto
        game_service.proto
    WORKING_DIRECTORY ${ZEROSUGAR_XR_SERVICE_DIR}
    DEPENDS zerosugar_proto_code_generator
)
