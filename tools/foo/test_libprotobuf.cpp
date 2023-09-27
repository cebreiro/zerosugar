#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <cstdint>
#include <memory>
#include <format>
#include <gtest/gtest.h>
#include <boost/algorithm/algorithm.hpp>

using namespace google::protobuf;
using namespace google::protobuf::compiler;

class MyCodeGenerator : public CodeGenerator
{
public:
    bool Generate(const FileDescriptor* file, const std::string& parameter,
        GeneratorContext* generator_context, std::string* error) const override
    {
        (void)parameter;
        (void)error;

        std::string fileName = file->name().substr(0, file->name().size() - 6);
        std::unique_ptr<io::ZeroCopyOutputStream> output(
            generator_context->OpenForInsert(std::format("{}_test.h", file->name()), ""));
        io::Printer printer(output.get(), '$');

        printer.Print("#pragma once\n");
        printer.Print(std::format("#include \"{}.pb.h\"\n", fileName).c_str());
        printer.Print("\n\n");

        for (int32_t i = 0; i < file->message_type_count(); ++i)
        {
            const Descriptor* message = file->message_type(i);
            printer.Print(std::format("class __{}\n", message->name()).c_str());
            printer.Print("{\n");
            printer.Print(std::format("\t {} _impl;\n", message->name()).c_str());
            printer.Print(std::format("public:\n").c_str());

            for (int32_t j = 0; j < message->field_count(); ++j)
            {
                const FieldDescriptor* field = message->field(j);

                printer.Print(std::format("\tauto __{0}() {{ return _impl.{0}(); }}\n", field->name()).c_str());
                printer.Print(std::format("\tvoid __set_{0}(google::protobuf::{1} value) {{ _impl.set_{0}(value); }}\n",
                    field->name(), field->cpp_type_name()).c_str());
                printer.Print(std::format("\tvoid __clear_{0}() {{ _impl.clear_{0}(); }}\n", field->name()).c_str());
                printer.Print("\n");
            }

            printer.Print("};\n");
            printer.Print("\n");
        }

        return true;
    }
};

int main(int argc, char* argv[])
{
    MyCodeGenerator generator;
    return PluginMain(argc, argv, &generator);
}
