add_executable (zerosugar_code_generator)

target_link_libraries(zerosugar_code_generator PUBLIC
    protobuf::libprotoc
    protobuf::libprotobuf
    Boost::boost
)

file(GLOB_RECURSE ZEROSUGAR_CODE_GENERATOR_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_code_generator PRIVATE ${ZEROSUGAR_CODE_GENERATOR_SRC_FILES})

set(ZEROSUGAR_BT_MODEL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/zerosugar/shared/ai/behavior_tree/model)
add_custom_target(zerosugar_generate_bt_model
    COMMAND ${ZEROSUGAR_PROTOBUF_PATH}
        --proto_path=./
        --plugin=protoc-gen-zerosugar_code_generator=$<TARGET_FILE_DIR:zerosugar_code_generator>/zerosugar_code_generator.exe
        --zerosugar_code_generator_out=./generated/
        --zerosugar_code_generator_opt=behavior_tree_model
        task.proto
    WORKING_DIRECTORY ${ZEROSUGAR_BT_MODEL_SOURCE_DIR}
)
add_dependencies(zerosugar_generate_bt_model zerosugar_code_generator)
