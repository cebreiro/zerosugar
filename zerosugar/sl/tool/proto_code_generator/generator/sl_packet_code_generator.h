#pragma once
#include "zerosugar/sl/tool/proto_code_generator/generator/code_generator_interface.h"

namespace zerosugar::sl
{
    class SLPacketCodeGenerator : public ICodeGenerator
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
    };
}
