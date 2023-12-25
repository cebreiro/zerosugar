#include "proto_code_generator.h"

#include <google/protobuf/compiler/code_generator.h>
#include <nlohmann/json.hpp>
#include <format>

#include "zerosugar/sl/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/sl/tool/proto_code_generator/generator/code_generator_factory.h"
#include "zerosugar/sl/tool/proto_code_generator/generator/code_generator_interface.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/input/converter/proto_input_converter.h"


using namespace google::protobuf;
using namespace google::protobuf::compiler;

namespace zerosugar::sl
{
    ProtoCodeGenerator::ProtoCodeGenerator()
        : _generatorFactory(std::make_unique<CodeGeneratorFactory>())
    {
    }

    ProtoCodeGenerator::~ProtoCodeGenerator()
    {
    }

    bool ProtoCodeGenerator::Generate(const FileDescriptor* file, const std::string& parameter,
        GeneratorContext* generator_context, std::string* error) const
    {
        try
        {
            nlohmann::json json = nlohmann::json::parse(parameter);
            const ProtoCodeGeneratorOption option = json.get<ProtoCodeGeneratorOption>();
            const WriterInputProtoConverter converter;
            const WriterInput input = converter.Convert(*file);

            if (std::unique_ptr<ICodeGenerator> generator = _generatorFactory->Create(option.generator))
            {
                generator->Generate(*file, *generator_context, option, input);
            }
            else
            {
                throw std::runtime_error(std::format("fail to find code generator. generator: {}", option.generator));
            }
        }
        catch (const std::exception& e)
        {
            *error = e.what();
            return false;
        }

        return true;
    }

    auto ProtoCodeGenerator::GetSupportedFeatures() const -> uint64_t
    {
        return FEATURE_PROTO3_OPTIONAL;
    }

    auto ProtoCodeGenerator::GetMessageFileName(const std::string& protoFileName) -> std::string
    {
        return protoFileName.substr(0, protoFileName.size() - 6) + "_generated";
    }
}
