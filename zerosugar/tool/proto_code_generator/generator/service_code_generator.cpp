#include "service_code_generator.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include "zerosugar/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/tool/proto_code_generator/writer/message_json_serialize_writer.h"
#include "zerosugar/tool/proto_code_generator/writer/message_writer.h"
#include "zerosugar/tool/proto_code_generator/writer/service_writer.h"
#include "zerosugar/tool/proto_code_generator/writer/input/writer_input.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;

namespace zerosugar
{
    void ServiceCodeGenerator::Generate(const FileDescriptor& file,
        GeneratorContext& context, const ProtoCodeGeneratorOption& option,
        const WriterInput& input)
    {
        _serviceFileName = GetServiceFileName(file.name());
        _messageFileName = GetMessageFileName(file.name());
        _messageJsonFileName = _messageFileName + "_json";

        GenerateService(file, context, option, input);
        GenerateMessage(file, context, option, input);
        GenerateMessageJsonSerialize(file, context, option, input);
    }

    auto ServiceCodeGenerator::GetName() const -> std::string
    {
        return "service_code_generator";
    }

    void ServiceCodeGenerator::GenerateService(const FileDescriptor& file,
        GeneratorContext& context, const ProtoCodeGeneratorOption& option,
        const WriterInput& input) const
    {
        (void)file;

        if (input.services.empty())
        {
            return;
        }

        const ServiceWriter::Param param = [&]()
            {
                std::vector<std::string> includes;
                includes.reserve(input.imports.size() + 1);

                includes.emplace_back(std::format("{}/{}.h", option.includePath, _messageFileName));

                for (const Import& imported : input.imports)
                {
                    includes.emplace_back(std::format("{}/{}.h", option.includePath,
                        GetMessageFileName(imported.name)));
                }

                return ServiceWriter::Param{
                    .input = input,
                    .headerName = _serviceFileName,
                    .messageJsonFileName = _messageJsonFileName,
                    .includes = std::move(includes),
                };
            }();

        ServiceWriter writer;
        const auto& [header, cpp] = writer.Write(param);
        {
            std::unique_ptr<io::ZeroCopyOutputStream> stream(
                context.OpenForInsert(std::format("{}.h", _serviceFileName), ""));
            io::Printer printer(stream.get(), '$');
            printer.Print(header.c_str());
        }
        {
            std::unique_ptr<io::ZeroCopyOutputStream> stream(
                context.OpenForInsert(std::format("{}.cpp", _serviceFileName), ""));
            io::Printer printer(stream.get(), '$');
            printer.Print(cpp.c_str());
        }
    }

    void ServiceCodeGenerator::GenerateMessage(const FileDescriptor& file,
        GeneratorContext& context, const ProtoCodeGeneratorOption& option,
        const WriterInput& input) const
    {
        (void)file;

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
                    .headerName = _messageFileName,
                    .includes = std::move(includes),
                };
            }();

        MessageWriter writer;
        const auto& [header, cpp] = writer.Write(param);

        {
            std::unique_ptr<io::ZeroCopyOutputStream> stream(
                context.OpenForInsert(std::format("{}.h", _messageFileName), ""));

            io::Printer printer(stream.get(), '$');
            printer.Print(header.c_str());
        }
        {
            if (!cpp.empty())
            {
                std::unique_ptr<io::ZeroCopyOutputStream> stream(
                    context.OpenForInsert(std::format("{}.cpp", _messageFileName), ""));

                io::Printer printer(stream.get(), '$');
                printer.Print(cpp.c_str());
            }
        }
    }

    void ServiceCodeGenerator::GenerateMessageJsonSerialize(const FileDescriptor& file,
        GeneratorContext& context, const ProtoCodeGeneratorOption& option,
        const WriterInput& input) const
    {
        (void)file;

        const MessageJsonSerializeWriter::Param param = [&]()
            {
                std::vector<std::string> includes;
                includes.reserve((input.imports.size() * 2) + 1);

                includes.emplace_back(std::format("{}/{}.h", option.includePath, _messageFileName));

                for (const Import& imported : input.imports)
                {
                    includes.emplace_back(std::format("{}/{}.h", option.includePath,
                        GetMessageFileName(imported.name)));
                    includes.emplace_back(std::format("{}/{}.h", option.includePath,
                        GetMessageFileName(imported.name) + "_json"));
                }

                return MessageJsonSerializeWriter::Param{
                    .input = input,
                    .headerName = _messageJsonFileName,
                    .includes = std::move(includes),
                };
            }();

        MessageJsonSerializeWriter writer;
        const auto& [header, cpp] = writer.Write(param);

        {
            std::unique_ptr<io::ZeroCopyOutputStream> stream(
                context.OpenForInsert(std::format("{}.h", _messageJsonFileName), ""));

            io::Printer printer(stream.get(), '$');
            printer.Print(header.c_str());
        }
        {
            std::unique_ptr<io::ZeroCopyOutputStream> stream(
                context.OpenForInsert(std::format("{}.cpp", _messageJsonFileName), ""));

            io::Printer printer(stream.get(), '$');
            printer.Print(cpp.c_str());
        }
    }
}
