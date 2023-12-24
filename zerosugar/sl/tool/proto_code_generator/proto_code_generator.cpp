#include "proto_code_generator.h"

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <nlohmann/json.hpp>
#include <memory>
#include <format>
#include <Windows.h>

#include "zerosugar/sl/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/message_json_serialize_writer.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/service_interface_writer.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/message_writer.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/input/converter/writer_input_proto_converter.h"

void DebugMessageBox(int32_t number)
{
    std::string str = std::format("{}", number);

    ::MessageBoxA(NULL, str.c_str(), "Debug", 0);
}

void DebugMessageBox(const std::string& str)
{
    ::MessageBoxA(NULL, str.c_str(), "Debug", 0);
}


using namespace google::protobuf;
using namespace google::protobuf::compiler;

namespace zerosugar::sl
{
    bool ProtoCodeGenerator::Generate(const FileDescriptor* file, const std::string& parameter,
        GeneratorContext* generator_context, std::string* error) const
    {
        try
        {
            nlohmann::json json = nlohmann::json::parse(parameter);
            const ProtoCodeGeneratorOption option = json.get<ProtoCodeGeneratorOption>();

            const WriterInputProtoConverter converter;
            const WriterInput input = converter.Convert(*file);

            GenerateServiceInterface(*file, *generator_context, option, input);
            GenerateMessage(*file, *generator_context, option, input);
            GenerateMessageJsonSerialize(*file, *generator_context, option, input);
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

    void ProtoCodeGenerator::GenerateServiceInterface(const FileDescriptor& file, GeneratorContext& context,
        const ProtoCodeGeneratorOption& option, const WriterInput& input) const
    {
        if (input.services.empty())
        {
            return;
        }

        const std::string& messageFileName = GetMessageFileName(file.name());
        const std::string& serviceInterfaceFileName = messageFileName + "_interface";
        const ServiceInterfaceWriter::Param param = [&]()
            {
                std::vector<std::string> includes;
                includes.reserve(input.imports.size() + 1);

                includes.emplace_back(std::format("{}/{}.h", option.includePath, messageFileName));

                for (const Import& imported : input.imports)
                {
                    includes.emplace_back(std::format("{}/{}.h", option.includePath,
                        GetMessageFileName(imported.name)));
                }

                return ServiceInterfaceWriter::Param{
                    .input = input,
                    .includes = std::move(includes),
                };
            }();

        std::unique_ptr<io::ZeroCopyOutputStream> stream(
            context.OpenForInsert(std::format("{}.h", serviceInterfaceFileName), ""));
        io::Printer printer(stream.get(), '$');

        ServiceInterfaceWriter writer;
        printer.Print(writer.Write(param).c_str());
    }

    void ProtoCodeGenerator::GenerateMessage(const FileDescriptor& file, GeneratorContext& context,
        const ProtoCodeGeneratorOption& option, const WriterInput& input) const
    {
        const MessageWriter::Param param = [&]()
            {
                std::vector<std::string> includes;
                includes.reserve(input.imports.size());

                for (const Import& imported : input.imports)
                {
                    includes.emplace_back(std::format("{}/{}.h", option.includePath,
                        GetMessageFileName(imported.name)));
                }

                return MessageWriter::Param{
                    .input = input,
                    .headerName = GetMessageFileName(file.name()),
                    .includes = std::move(includes),
                };
            }();

        std::unique_ptr<io::ZeroCopyOutputStream> stream(
            context.OpenForInsert(std::format("{}.h", GetMessageFileName(file.name())), ""));
        io::Printer printer(stream.get(), '$');

        MessageWriter writer;
        printer.Print(writer.Write(param).c_str());
    }

    void ProtoCodeGenerator::GenerateMessageJsonSerialize(const FileDescriptor& file, GeneratorContext& context,
        const ProtoCodeGeneratorOption& option, const WriterInput& input) const
    {
        const std::string& messageFileName = GetMessageFileName(file.name());
        const std::string& jsonSerializeFileName = messageFileName + "_json_serialize";

        const MessageJsonSerializeWriter::Param param = [&]()
            {
                std::vector<std::string> includes;
                includes.reserve((input.imports.size() * 2) + 1);

                includes.emplace_back(std::format("{}/{}.h", option.includePath, messageFileName));

                for (const Import& imported : input.imports)
                {
                    includes.emplace_back(std::format("{}/{}.h", option.includePath,
                        GetMessageFileName(imported.name)));
                    includes.emplace_back(std::format("{}/{}.h", option.includePath,
                        GetMessageFileName(imported.name) + "_json_serialize"));
                }

                return MessageJsonSerializeWriter::Param{
                    .input = input,
                    .headerName = jsonSerializeFileName,
                    .includes = std::move(includes),
                };
            }();

        MessageJsonSerializeWriter writer;
        const auto& [header, cpp] = writer.Write(param);

        {
            std::unique_ptr<io::ZeroCopyOutputStream> stream(
                context.OpenForInsert(std::format("{}.h", jsonSerializeFileName), ""));

            io::Printer printer(stream.get(), '$');
            printer.Print(header.c_str());
        }
        {
            std::unique_ptr<io::ZeroCopyOutputStream> stream(
                context.OpenForInsert(std::format("{}.cpp", jsonSerializeFileName), ""));

            io::Printer printer(stream.get(), '$');
            printer.Print(cpp.c_str());
        }
    }

    auto ProtoCodeGenerator::GetMessageFileName(const std::string& protoFileName) -> std::string
    {
        return protoFileName.substr(0, protoFileName.size() - 6) + "_generated";
    }
}
