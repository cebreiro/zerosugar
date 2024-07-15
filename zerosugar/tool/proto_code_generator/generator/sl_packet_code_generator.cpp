#include "sl_packet_code_generator.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include "zerosugar/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/tool/proto_code_generator/writer/sl_message_writer.h"
#include "zerosugar/tool/proto_code_generator/writer/sl_packet_writer.h"
#include "zerosugar/tool/proto_code_generator/writer/input/writer_input.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;

namespace zerosugar
{
    void SLPacketCodeGenerator::Generate(const FileDescriptor& file,
        GeneratorContext& context, const ProtoCodeGeneratorOption& option,
        const WriterInput& input)
    {
        GenerateMessage(file, context, option, input);
        GeneratePacket(file, context, option, input);
    }

    auto SLPacketCodeGenerator::GetName() const -> std::string
    {
        return "sl_packet_code_generator";
    }

    void SLPacketCodeGenerator::GenerateMessage(const FileDescriptor& file,
        GeneratorContext& context, const ProtoCodeGeneratorOption& option,
        const WriterInput& input) const
    {
        const SlMessageWriter::Param param = [&]()
            {
                std::vector<std::string> includes;
                includes.reserve(input.imports.size());

                for (const Import& imported : input.imports)
                {
                    if (imported.name.starts_with("option"))
                    {
                        continue;
                    }

                    includes.emplace_back(fmt::format("{}/{}.h", option.includePath,
                        GetMessageFileName(imported.name)));
                }

                return SlMessageWriter::Param{
                    .input = input,
                    .headerName = GetMessageFileName(file.name()),
                    .includes = std::move(includes),
                };
            }();

        SlMessageWriter writer;
        if (!writer.CanWrite(param))
        {
            return;
        }

        const auto& [header, cpp] = writer.Write(param);

        {
            std::unique_ptr<io::ZeroCopyOutputStream> stream(
                context.OpenForInsert(fmt::format("{}.h", GetMessageFileName(file.name())), ""));

            io::Printer printer(stream.get(), '$');
            printer.Print(header.c_str());
        }
        {
            std::unique_ptr<io::ZeroCopyOutputStream> stream(
                context.OpenForInsert(fmt::format("{}.cpp", GetMessageFileName(file.name())), ""));

            io::Printer printer(stream.get(), '$');
            printer.Print(cpp.c_str());
        }
    }

    void SLPacketCodeGenerator::GeneratePacket(const FileDescriptor& file,
        GeneratorContext& context, const ProtoCodeGeneratorOption& option,
        const WriterInput& input) const
    {
        const SlPacketWriter::Param param = [&]()
            {
                std::vector<std::string> includes;
                includes.reserve(input.imports.size());

                for (const Import& imported : input.imports)
                {
                    if (imported.name.starts_with("option"))
                    {
                        continue;
                    }

                    includes.emplace_back(fmt::format("{}/{}.h", option.includePath,
                        GetMessageFileName(imported.name)));
                }

                return SlPacketWriter::Param{
                    .input = input,
                    .headerName = GetMessageFileName(file.name()),
                    .includes = std::move(includes),
                };
            }();

        SlPacketWriter writer;
        if (!writer.CanWrite(param))
        {
            return;
        }

        const auto& [header, cpp] = writer.Write(param);

        {
            std::unique_ptr<io::ZeroCopyOutputStream> stream(
                context.OpenForInsert(fmt::format("{}.h", GetMessageFileName(file.name())), ""));

            io::Printer printer(stream.get(), '$');
            printer.Print(header.c_str());
        }
        {
            std::unique_ptr<io::ZeroCopyOutputStream> stream(
                context.OpenForInsert(fmt::format("{}.cpp", GetMessageFileName(file.name())), ""));

            io::Printer printer(stream.get(), '$');
            printer.Print(cpp.c_str());
        }
    }
}
