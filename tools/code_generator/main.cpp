#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <cstdint>
#include <sstream>
#include <memory>
#include <format>
#include <boost/algorithm/string.hpp>

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/type/not_null_pointer.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;

static constexpr const char* line_feed = "\r\n";

class Printer
{
public:
    explicit Printer(io::Printer& impl)
        : _impl(impl)
    {
    }

    template <typename... Args>
    void AddInclude(const std::string& header, Args&&... args)
    {
        if constexpr (sizeof...(Args) == 0)
        {
            _impl.Print(std::format("#include {}{}", header, line_feed).c_str());
        }
        else
        {
            auto formatStr = std::vformat(header, std::make_format_args(std::forward<Args>(args)...));
            _impl.Print(std::format("#include {}{}", std::move(formatStr), line_feed).c_str());
        }
    }

    void BreakLine()
    {
        _impl.Print(line_feed);
    }

    template <typename... Args>
    void AddLine(size_t indent, const std::string& str, Args&&... args)
    {
        if (indent == 0)
        {
            _impl.Print(std::format("{}\r\n", str).c_str());
            return;
        }

        std::ostringstream oss;
        for (size_t i = 0; i < indent; ++i)
        {
            oss << "    ";
        }

        if constexpr (sizeof...(Args) == 0)
        {
            _impl.Print(std::format("{}{}{}", oss.str(), str, line_feed).c_str());
        }
        else
        {
            auto formatStr = std::vformat(str, std::make_format_args(std::forward<Args>(args)...));
            _impl.Print(std::format("{}{}{}", oss.str(), std::move(formatStr), line_feed).c_str());
        }
    }

private:
    io::Printer& _impl;
};

auto ToSnakeCase(std::string str) -> std::string
{
    for (int i = 1; i < str.length(); i++)
    {
        if (isupper(str[i]))
        {
            str.insert(i, "_");
            ++i;
        }
    }

    return boost::algorithm::to_lower_copy(str);
}

auto ConvertPackageNameToNamespace(const std::string& str) -> std::string
{
    std::string result(str);
    boost::algorithm::replace_all(result, ".", "::");
    boost::algorithm::to_lower(result);

    return result;
}

auto ToXmlAttributeMethodName(const std::string& protoCppType) -> std::string
{
    if (protoCppType == "int32")
    {
        return "as_int";
    }
    else if (protoCppType == "int64")
    {
        return "as_llong";
    }
    else if (protoCppType == "uint32")
    {
        return "as_uint";
    }
    else if (protoCppType == "uint64")
    {
        return "as_ullong";
    }

    return std::format("as_{}", protoCppType);
}

auto ToCppType(const std::string& protoCppType) -> std::string
{
    if (protoCppType == "int32")
    {
        return "int32_t";
    }
    else if (protoCppType == "int64")
    {
        return "int64_t";
    }
    else if (protoCppType == "uint32")
    {
        return "uint32_t";
    }
    else if (protoCppType == "uint64")
    {
        return "uint64_t";
    }

    return std::format("{}", protoCppType);
}

auto GetCppMemberTypeName(const FieldDescriptor& field) -> std::string
{
    if (field.message_type())
    {
        return std::format("std::unique_ptr<{}>", field.message_type()->name());
    }

    return std::format("{}_t", field.cpp_type_name());
}

void PrintHeader(const FileDescriptor& fileDescriptor, Printer& printer)
{
    printer.AddLine(0, "#pragma once");
    printer.AddInclude("<cstdint>");
    printer.AddInclude("<memory>");
    printer.AddInclude("<vector>");
    printer.AddInclude("\"zerosugar/shared/ai/behavior_tree/model/model.h\"");
    printer.BreakLine();

    printer.AddLine(0, std::format("namespace {}", ConvertPackageNameToNamespace(fileDescriptor.package())));
    printer.AddLine(0, "{");

    for (int32_t i = 0; i < fileDescriptor.message_type_count(); ++i)
    {
        const Descriptor* md = fileDescriptor.message_type(i);
        std::vector<const FieldDescriptor*> fields;

        for (int32_t j = 0; j < md->field_count(); ++j)
        {
            const auto* field = md->field(j);
            if (!field)
            {
                continue;
            }

            fields.push_back(field);
        }

        printer.AddLine(1, "class {} : public Model", md->name());
        printer.AddLine(1, "{");
        printer.AddLine(1, "public:");
        printer.AddLine(2, "static constexpr const char* class_name = \"{}\";",
            ToSnakeCase(md->name()));
        printer.BreakLine();
        printer.AddLine(2, "bool Deserialize(const pugi::xml_node& node) override;");
        printer.BreakLine();

        printer.AddLine(1, "protected:");

        for (const FieldDescriptor& field : fields | zerosugar::notnull::reference)
        {
            if (field.is_repeated())
            {
                printer.AddLine(2, "std::vector<{}> _{};",
                    GetCppMemberTypeName(field), field.name());
            }
            else
            {
                printer.AddLine(2, "{} _{} = {{}};",
                    GetCppMemberTypeName(field), field.name());
            }
        }

        printer.BreakLine();
        printer.AddLine(2, "static ModelFactory::Registry<{}> _registry;", md->name());

        printer.AddLine(1, "};");
        printer.BreakLine();
    }

    printer.AddLine(0, "}");
}

void PrintCpp(const FileDescriptor& fileDescriptor, Printer& printer)
{
    printer.AddInclude("\"{}.h\"", fileDescriptor.name());
    printer.BreakLine();

    printer.AddInclude("\"<cassert>\"", fileDescriptor.name());
    printer.AddInclude("\"<cstring>\"", fileDescriptor.name());
    printer.BreakLine();

    printer.AddLine(0, std::format("namespace {}", ConvertPackageNameToNamespace(fileDescriptor.package())));
    printer.AddLine(0, "{");

    for (int32_t i = 0; i < fileDescriptor.message_type_count(); ++i)
    {
        const Descriptor* md = fileDescriptor.message_type(i);
        std::vector<const FieldDescriptor*> fields;

        for (int32_t j = 0; j < md->field_count(); ++j)
        {
            const auto* field = md->field(j);
            if (!field)
            {
                continue;
            }

            fields.push_back(field);
        }

        printer.AddLine(1, "bool {}::Deserialize(const pugi::xml_node& node)", md->name());
        printer.AddLine(1, "{");
        printer.BreakLine();
        printer.AddLine(2, "assert(!::_stricmp(node.name(), class_name));");
        printer.AddLine(2, "if (!Model::Deserialize(node))");
        printer.AddLine(2, "{");
        printer.AddLine(3, "return false;");
        printer.AddLine(2, "}");
        printer.BreakLine();

        for (const FieldDescriptor& field : fields | zerosugar::notnull::reference)
        {
            const bool modelField = field.message_type() ?
                field.message_type()->name() == "Model" : false;

            if (field.is_repeated())
            {
                if (modelField)
                {
                    printer.AddLine(2, "for (const pugi::xml_node& child : node.children())");
                    printer.AddLine(2, "{");
                    printer.AddLine(3, "auto model = ModelFactory::GetInstance().CreateModel(child);");
                    printer.AddLine(3, "if (!model)");
                    printer.AddLine(3, "{");
                    printer.AddLine(4, "return false;");
                    printer.AddLine(3, "}");
                    printer.AddLine(3, "_{}.emplace_back(std::move(model));", field.name());
                    printer.AddLine(2, "}");
                }
                else
                {
                    printer.AddLine(3, "if (auto opt = Param::As<std::vector<{}>>(node.attribute(\"{}\")); opt.has_value())",
                        ToCppType(field.cpp_type_name()), field.name());
                    printer.AddLine(3, "{");
                    printer.AddLine(4, "_{} = std::move(*opt);");
                    printer.AddLine(3, "}");
                    printer.AddLine(3, "else");
                    printer.AddLine(3, "{");
                    printer.AddLine(4, "return false;");
                    printer.AddLine(3, "}");
                }
            }
            else
            {
                if (modelField)
                {
                    printer.AddLine(2, "_{} = ModelFactory::GetInstance().CreateModel(node.first_child());", field.name());
                    printer.AddLine(2, "if (!_{})", field.name());
                    printer.AddLine(2, "{");
                    printer.AddLine(3, "return false;");
                    printer.AddLine(2, "}");
                }
                else
                {
                    printer.AddLine(2, "if (auto opt = Param::As<{}>(node.attribute(\"{}\")); opt.has_value())",
                        ToCppType(field.cpp_type_name()), field.name());
                    printer.AddLine(2, "{");
                    printer.AddLine(3, "_{} = std::move(*opt);", field.name());
                    printer.AddLine(2, "}");
                    printer.AddLine(2, "else");
                    printer.AddLine(2, "{");
                    printer.AddLine(3, "return false;");
                    printer.AddLine(2, "}");
                }
            }
            printer.BreakLine();
        }
        printer.AddLine(2, "return true;");
        printer.AddLine(1, "}");

        printer.BreakLine();
        printer.AddLine(1, "ModelFactory::Registry<{0}> {0}::_registry;", md->name());
        printer.BreakLine();
    }

    printer.AddLine(0, "}");
}

class BehaviorTreeModelGenerator : public google::protobuf::compiler::CodeGenerator
{
public:
    bool Generate(const FileDescriptor* file, const std::string& parameter,
        GeneratorContext* generator_context, std::string* error) const override
    {
        if (parameter != "behavior_tree_model")
        {
            return false;
        }
        
        (void)parameter;
        (void)error;

        std::string fileName = file->name().substr(0, file->name().size() - 6);
        {
            std::unique_ptr<io::ZeroCopyOutputStream> output(
                generator_context->OpenForInsert(std::format("{}.h", file->name()), ""));
            io::Printer pbPrinter(output.get(), '$');

            Printer printer(pbPrinter);
            PrintHeader(*file, printer);
        }
        {
            std::unique_ptr<io::ZeroCopyOutputStream> output(
                generator_context->OpenForInsert(std::format("{}.cpp", file->name()), ""));
            io::Printer pbPrinter(output.get(), '$');

            Printer printer(pbPrinter);
            PrintCpp(*file, printer);
        }

        return true;
    }
};

int main(int argc, char* argv[])
{
    BehaviorTreeModelGenerator generator;
    return PluginMain(argc, argv, &generator);
}
