#pragma once
#include "zerosugar/tool/proto_code_generator/generator/code_generator_interface.h"

namespace zerosugar
{
    class XRPacketCodeGenerator : public ICodeGenerator
    {
    public:
        void Generate(const google::protobuf::FileDescriptor& file,
            google::protobuf::compiler::GeneratorContext& context, const ProtoCodeGeneratorOption& option,
            const WriterInput& input) override;
        auto GetName() const->std::string override;

    private:
        void GenerateMessage(
            const google::protobuf::FileDescriptor& file,
            google::protobuf::compiler::GeneratorContext& context,
            const ProtoCodeGeneratorOption& option, const WriterInput& input) const;
        void GeneratePacket(
            const google::protobuf::FileDescriptor& file,
            google::protobuf::compiler::GeneratorContext& context,
            const ProtoCodeGeneratorOption& option, const WriterInput& input) const;
        void GenerateMessageJsonSerialize(const google::protobuf::FileDescriptor& file,
            google::protobuf::compiler::GeneratorContext& context,
            const ProtoCodeGeneratorOption& option, const WriterInput& input) const;

    private:
        std::string _messageFileName;
        std::string _messageJsonFileName;
    };
}
