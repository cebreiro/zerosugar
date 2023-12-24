#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <memory>
#include <format>

#include "zerosugar/shared/type/not_null_pointer.h"
#include <Windows.h>

#include "zerosugar/sl/tool/proto_code_generator/writer/service_interface_writer.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/service_message_writer.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/input/converter/writer_input_proto_converter.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;

void DebugMessageBox(int32_t number)
{
    std::string str = std::format("{}", number);

    ::MessageBoxA(NULL, str.c_str(), "Debug", 0);
}

void DebugMessageBox(const std::string& str)
{
    ::MessageBoxA(NULL, str.c_str(), "Debug", 0);
}

auto GetProtoExtensionRemovedName(const std::string& fileName) -> std::string
{
    return fileName.substr(0, fileName.size() - 6) + "_generated";
}

namespace zerosugar::sl
{
    class ServiceCodeGenerator : public CodeGenerator
    {
    public:
        bool Generate(const FileDescriptor* file, const std::string& parameter,
            GeneratorContext* generator_context, std::string* error) const override
        {
            try
            {
                WriterInputProtoConverter converter;
                WriterInput input = converter.Convert(*file);

                const std::string fileName = GetProtoExtensionRemovedName(file->name());
                const std::string serviceMessageFileName = fileName;

                if (!input.services.empty())
                {
                    const std::string serviceInterfaceFileName = fileName + "_interface";
                    ServiceInterfaceWriter interfaceWriter;

                    std::unique_ptr<io::ZeroCopyOutputStream> stream(
                        generator_context->OpenForInsert(std::format("{}.h", serviceInterfaceFileName), ""));
                    io::Printer printer(stream.get(), '$');

                    ServiceInterfaceWriter::Param param{
                        .input = input,
                        .includes = {
                            std::format("{}/{}.h", parameter, serviceMessageFileName),
                        },
                    };

                    for (const Import& imported : input.imports)
                    {
                        param.includes.emplace_back(std::format("{}/{}.h", parameter,
                            GetProtoExtensionRemovedName(imported.name)));
                    }

                    printer.Print(interfaceWriter.Write(param).c_str());
                }

                {
                    ServiceMessageWriter messageWriter;
                    ServiceMessageWriter::Param param{
                        .input = input,
                        .headerName = serviceMessageFileName,
                    };

                    for (const Import& imported : input.imports)
                    {
                        param.includes.emplace_back(std::format("{}/{}.h", parameter,
                            GetProtoExtensionRemovedName(imported.name)));
                    }

                    const auto& [header, cxx] = messageWriter.Write(param);

                    {
                        std::unique_ptr<io::ZeroCopyOutputStream> stream(
                            generator_context->OpenForInsert(std::format("{}.h", serviceMessageFileName), ""));
                        io::Printer printer(stream.get(), '$');

                        printer.Print(header.c_str());
                    }
                    {
                        std::unique_ptr<io::ZeroCopyOutputStream> stream(
                            generator_context->OpenForInsert(std::format("{}.cpp", serviceMessageFileName), ""));
                        io::Printer printer(stream.get(), '$');

                        printer.Print(cxx.c_str());
                    }
                }
            }
            catch (const std::exception& e)
            {
                *error = e.what();
                return false;
            }

            return true;
        }

    private:
        uint64_t GetSupportedFeatures() const override
        {
            return FEATURE_PROTO3_OPTIONAL;
        }
    };
}

int main(int argc, char* argv[])
{
    zerosugar::sl::ServiceCodeGenerator generator;
    return PluginMain(argc, argv, &generator);
}
