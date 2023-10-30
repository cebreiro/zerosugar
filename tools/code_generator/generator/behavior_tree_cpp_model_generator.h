#pragma once
#include "tools/code_generator/generator/generator_interface.h"
#include "tools/code_generator/generator/generator_factory.h"

namespace google::protobuf
{
    class FieldDescriptor;
}

namespace zerosugar::generator
{
    class BehaviorTreeCppModelGenerator : public ICodeGenerator
    {
    public:
        static constexpr const char* name = "behavior_tree_model";

    public:
        auto Generate(const google::protobuf::FileDescriptor& fileDescriptor) const -> Result override;

    private:
        auto GenerateHeader(const google::protobuf::FileDescriptor& fileDescriptor) const  -> std::pair<std::filesystem::path, std::string>;
        auto GenerateCpp(const std::string& headerName, const google::protobuf::FileDescriptor& fileDescriptor) const -> std::string;


        static bool IsModel(const google::protobuf::FieldDescriptor& field);
        static auto ToCppType(const google::protobuf::FieldDescriptor& field) -> std::string;

    private:
        static GeneratorFactory::Registry<BehaviorTreeCppModelGenerator> _registry;
    };
}
