#include "service_code_generator.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include "zerosugar/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/tool/proto_code_generator/writer/message_json_serialize_writer.h"
#include "zerosugar/tool/proto_code_generator/writer/message_writer.h"
#include "zerosugar/tool/proto_code_generator/writer/service_interface_writer.h"
#include "zerosugar/tool/proto_code_generator/writer/input/writer_input.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;

namespace zerosugar
{
    void ServiceCodeGenerator::Generate(const FileDescriptor& file,
        GeneratorContext& context, const ProtoCodeGeneratorOption& option,
        const WriterInput& input)
    {
        GenerateServiceInterface(file, context, option, input);
        GenerateMessage(file, context, option, input);
        GenerateMessageJsonSerialize(file, context, option, input);
    }

    auto ServiceCodeGenerator::GetName() const -> std::string
    {
        return "service_code_generator";
    }

    void ServiceCodeGenerator::GenerateServiceInterface(const FileDescriptor& file,
        GeneratorContext& context, const ProtoCodeGeneratorOption& option,
        const WriterInput& input) const
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

    void ServiceCodeGenerator::GenerateMessage(const FileDescriptor& file,
        GeneratorContext& context, const ProtoCodeGeneratorOption& option,
        const WriterInput& input) const
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
                    .includes = std::move(includes),
                };
            }();

        std::unique_ptr<io::ZeroCopyOutputStream> stream(
            context.OpenForInsert(std::format("{}.h", GetMessageFileName(file.name())), ""));
        io::Printer printer(stream.get(), '$');

        MessageWriter writer;
        printer.Print(writer.Write(param).c_str());
    }

    void ServiceCodeGenerator::GenerateMessageJsonSerialize(const FileDescriptor& file,
        GeneratorContext& context, const ProtoCodeGeneratorOption& option,
        const WriterInput& input) const
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
}
