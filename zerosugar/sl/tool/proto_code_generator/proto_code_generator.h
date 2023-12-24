#pragma once
#include <google/protobuf/compiler/code_generator.h>

namespace zerosugar::sl
{
    struct ProtoCodeGeneratorOption;
    struct WriterInput;

    class ProtoCodeGenerator : public google::protobuf::compiler::CodeGenerator
    {
    public:
        bool Generate(const google::protobuf::FileDescriptor* file,
            const std::string& parameter,
            google::protobuf::compiler::GeneratorContext* generator_context,
            std::string* error) const override;

    private:
        auto GetSupportedFeatures() const -> uint64_t override;

    private:
        void GenerateServiceInterface(
            const google::protobuf::FileDescriptor& file,
            google::protobuf::compiler::GeneratorContext& generator_context,
            const ProtoCodeGeneratorOption& option, const WriterInput& input) const;
        void GenerateMessage(const google::protobuf::FileDescriptor& file,
            google::protobuf::compiler::GeneratorContext& generator_context,
            const ProtoCodeGeneratorOption& option, const WriterInput& input) const;
        void GenerateMessageJsonSerialize(const google::protobuf::FileDescriptor& file,
            google::protobuf::compiler::GeneratorContext& generator_context,
            const ProtoCodeGeneratorOption& option, const WriterInput& input) const;

    private:
        static auto GetMessageFileName(const std::string& protoFileName) -> std::string;
    };
}
