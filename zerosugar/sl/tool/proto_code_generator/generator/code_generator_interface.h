#pragma once
#include <string>
#include "zerosugar/sl/tool/proto_code_generator/generator/code_generator_util.h"

namespace google::protobuf
{
    class FileDescriptor;
}

namespace google::protobuf::compiler
{
    class GeneratorContext;
}

namespace zerosugar::sl
{
    struct ProtoCodeGeneratorOption;
    struct WriterInput;

    class ICodeGenerator
    {
    public:
        ICodeGenerator() = default;
        virtual ~ICodeGenerator() = default;

        virtual void Generate(const google::protobuf::FileDescriptor& file, google::protobuf::compiler::GeneratorContext& context,
            const ProtoCodeGeneratorOption& option, const WriterInput& input) = 0;
        virtual auto GetName() const -> std::string = 0;
    };
}
