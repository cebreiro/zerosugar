#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <memory>
#include <format>
#include <Windows.h>

#include "tools/code_generator/generator/generator_factory.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;

namespace zerosugar
{
    class ProtoCodeGenerator : public google::protobuf::compiler::CodeGenerator
    {
    public:
        bool Generate(const FileDescriptor* file, const std::string& parameter,
            GeneratorContext* generator_context, std::string* error) const override
        {
            (void)error;

            auto generator = GeneratorFactory::GetInstance().CreateGenerator(parameter);
            if (!generator)
            {
                std::string message = std::format("fail to find generator from paramter=\"{}\"", parameter);
                ::MessageBoxA(NULL, message.c_str(), "ProtoCodeGnerator Error", NULL);

                return false;
            }

            const generator::Result& result = generator->Generate(*file);

            auto printHeader = [&]()
                {
                    if (result.headerName.empty() || result.header.empty())
                    {
                        return false;
                    }

                    std::unique_ptr<io::ZeroCopyOutputStream> output(
                        generator_context->OpenForInsert(result.headerName.string(), ""));
                    io::Printer printer(output.get(), '$');

                    printer.Print(result.header.c_str());

                    return true;
                };

            auto printCpp = [&]()
                {
                    if (result.cpp.empty())
                    {
                        return false;
                    }

                    std::unique_ptr<io::ZeroCopyOutputStream> output(
                        generator_context->OpenForInsert(std::format("{}.cpp", result.headerName.stem().string()), ""));
                    io::Printer printer(output.get(), '$');

                    printer.Print(result.cpp.c_str());

                    return true;
                };

            bool printResult = false;

            printResult |= printHeader();
            printResult |= printCpp();

            return printResult;
        }
    };
}

int main(int argc, char* argv[])
{
    zerosugar::ProtoCodeGenerator generator;
    return PluginMain(argc, argv, &generator);
}
