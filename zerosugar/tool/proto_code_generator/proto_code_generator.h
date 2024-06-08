#pragma once
#include <memory>
#include <google/protobuf/compiler/code_generator.h>

namespace zerosugar
{
    struct ProtoCodeGeneratorOption;
    struct WriterInput;
    class CodeGeneratorFactory;

    class ProtoCodeGenerator : public google::protobuf::compiler::CodeGenerator
    {
    public:
        ProtoCodeGenerator();
        ~ProtoCodeGenerator();

        bool Generate(const google::protobuf::FileDescriptor* file,
            const std::string& parameter,
            google::protobuf::compiler::GeneratorContext* generator_context,
            std::string* error) const override;

    private:
        auto GetSupportedFeatures() const -> uint64_t override;

    private:
        static auto GetMessageFileName(const std::string& protoFileName) -> std::string;

    private:
        std::unique_ptr<CodeGeneratorFactory> _generatorFactory;
    };
}
