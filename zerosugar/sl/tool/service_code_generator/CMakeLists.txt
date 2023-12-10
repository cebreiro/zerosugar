add_executable (zerosugar_sl_generator_service_code)

target_link_libraries(zerosugar_sl_generator_service_code PUBLIC
    protobuf::libprotoc
    protobuf::libprotobuf
    Boost::boost
)

file(GLOB_RECURSE ZEROSUGAR_SL_GENERATOR_SERVICE_CODE_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_sl_generator_service_code PRIVATE ${ZEROSUGAR_SL_GENERATOR_SERVICE_CODE_SRC_FILES})

set(ZEROSUGAR_SERVICE_GENERATE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/zerosugar/sl/service/model/)

add_custom_target(zerosugar_sl_generate_service_code
    COMMAND ${ZEROSUGAR_PROTOBUF_PATH}
        --proto_path=${ZEROSUGAR_PROTOBUF_INCLUDE}
        --proto_path=./
        --plugin=protoc-gen-zerosugar_sl_generator_service_code=$<TARGET_FILE_DIR:zerosugar_sl_generator_service_code>/zerosugar_sl_generator_service_code.exe
        --zerosugar_sl_generator_service_code_opt=zerosugar/sl/service/generated #[[ for include directory ]]
        --zerosugar_sl_generator_service_code_out=../generated
        shared.proto
        config.proto
        login_service.proto
        repository_service.proto
    WORKING_DIRECTORY ${ZEROSUGAR_SERVICE_GENERATE_SOURCE_DIR}
)
