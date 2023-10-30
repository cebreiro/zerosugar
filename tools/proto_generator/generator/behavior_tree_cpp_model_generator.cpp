#include "behavior_tree_cpp_model_generator.h"

#include <ranges>
#include <google/protobuf/descriptor.h>

#include "tools/proto_generator/text_writer.h"
#include "tools/proto_generator/proto_util.h"
#include "tools/proto_generator/string_util.h"

using google::protobuf::FileDescriptor;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;

namespace
{
    constexpr auto field_deref = [](const FieldDescriptor* field) -> const FieldDescriptor&
        {
            return *field;
        };
}

namespace zerosugar::generator
{
    GeneratorFactory::Registry<BehaviorTreeCppModelGenerator> BehaviorTreeCppModelGenerator::_registry;

    auto BehaviorTreeCppModelGenerator::Generate(const FileDescriptor& fileDescriptor) const -> Result
    {
        Result result;

        std::tie(result.headerName, result.header) = GenerateHeader(fileDescriptor);
        result.cpp = GenerateCpp(result.headerName.filename().stem().string(), fileDescriptor);

        return result;
    }

    auto BehaviorTreeCppModelGenerator::GenerateHeader(const FileDescriptor& fileDescriptor) const -> std::pair<std::filesystem::path, std::string>
    {
        TextWriter writer;
        writer.WriteLine(0, "#pragma once");
        writer.WriteLine(0, "#include <cstdint>");
        writer.WriteLine(0, "#include <memory>");
        writer.WriteLine(0, "#include <vector>");
        writer.WriteLine(0, "#include \"zerosugar/shared/ai/behavior_tree/model/model.h\"");
        writer.BreakLine();

        writer.WriteLine(0, std::format("namespace {}", proto_util::ConvertPackageToNamespace(fileDescriptor.package())));
        writer.WriteLine(0, "{");

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

            writer.WriteLine(1, "class {} : public Model", md->name());
            writer.WriteLine(1, "{");
            writer.WriteLine(1, "public:");
            writer.WriteLine(2, "static constexpr const char* class_name = \"{}\";",
                string_util::ToSnakeCase(md->name()));
            writer.BreakLine();
            writer.WriteLine(2, "bool Deserialize(const pugi::xml_node& node) override;");
            writer.BreakLine();

            writer.WriteLine(1, "protected:");

            for (const FieldDescriptor& field : fields | std::views::transform(field_deref))
            {
                std::string typeName = ToCppType(field);
                if (IsModel(field))
                {
                    typeName = std::format("std::unique_ptr<{}>", typeName);
                }

                if (field.is_repeated())
                {
                    writer.WriteLine(2, "std::vector<{}> _{};",
                        typeName, field.name());
                }
                else
                {
                    writer.WriteLine(2, "{} _{} = {{}};",
                        typeName, field.name());
                }
            }

            writer.BreakLine();
            writer.WriteLine(2, "static ModelFactory::Registry<{}> _registry;", md->name());

            writer.WriteLine(1, "};");
            writer.BreakLine();
        }

        writer.WriteLine(0, "}");

        std::filesystem::path headerName = std::format("{}.h", fileDescriptor.name());
        std::string header = writer.GetString();

        return std::pair<std::filesystem::path, std::string>(std::move(headerName), std::move(header));
    }

    auto BehaviorTreeCppModelGenerator::GenerateCpp(const std::string& headerName,
        const FileDescriptor& fileDescriptor) const -> std::string
    {
        TextWriter writer;
        writer.WriteLine(0, "#include \"{}.h\"", headerName);
        writer.BreakLine();

        writer.WriteLine(0, "#include <cassert>");
        writer.WriteLine(0, "#include <cstring>");
        writer.BreakLine();

        writer.WriteLine(0, std::format("namespace {}", proto_util::ConvertPackageToNamespace(fileDescriptor.package())));
        writer.WriteLine(0, "{");

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

            writer.WriteLine(1, "bool {}::Deserialize(const pugi::xml_node& node)", md->name());
            writer.WriteLine(1, "{");
            writer.WriteLine(2, "assert(!::_stricmp(node.name(), class_name));");
            writer.BreakLine();
            writer.WriteLine(2, "if (!Model::Deserialize(node))");
            writer.WriteLine(2, "{");
            writer.WriteLine(3, "return false;");
            writer.WriteLine(2, "}");
            writer.BreakLine();

            for (const FieldDescriptor& field : fields | std::views::transform(field_deref))
            {
                if (field.is_repeated())
                {
                    if (IsModel(field))
                    {
                        writer.WriteLine(2, "for (const pugi::xml_node& child : node.children())");
                        writer.WriteLine(2, "{");
                        writer.WriteLine(3, "auto model = ModelFactory::GetInstance().CreateModel(child);");
                        writer.WriteLine(3, "if (!model)");
                        writer.WriteLine(3, "{");
                        writer.WriteLine(4, "return false;");
                        writer.WriteLine(3, "}");
                        writer.WriteLine(3, "_{}.emplace_back(std::move(model));", field.name());
                        writer.WriteLine(2, "}");
                    }
                    else
                    {
                        writer.WriteLine(3, "if (auto opt = Param::As<std::vector<{}>>(node.attribute(\"{}\")); opt.has_value())",
                            ToCppType(field), field.name());
                        writer.WriteLine(3, "{");
                        writer.WriteLine(4, "_{} = std::move(*opt);");
                        writer.WriteLine(3, "}");
                        writer.WriteLine(3, "else");
                        writer.WriteLine(3, "{");
                        writer.WriteLine(4, "return false;");
                        writer.WriteLine(3, "}");
                    }
                }
                else
                {
                    if (IsModel(field))
                    {
                        writer.WriteLine(2, "_{} = ModelFactory::GetInstance().CreateModel(node.first_child());", field.name());
                        writer.WriteLine(2, "if (!_{})", field.name());
                        writer.WriteLine(2, "{");
                        writer.WriteLine(3, "return false;");
                        writer.WriteLine(2, "}");
                    }
                    else
                    {
                        writer.WriteLine(2, "if (auto opt = Param::As<{}>(node.attribute(\"{}\")); opt.has_value())",
                            ToCppType(field), field.name());
                        writer.WriteLine(2, "{");
                        writer.WriteLine(3, "_{} = std::move(*opt);", field.name());
                        writer.WriteLine(2, "}");
                        writer.WriteLine(2, "else");
                        writer.WriteLine(2, "{");
                        writer.WriteLine(3, "return false;");
                        writer.WriteLine(2, "}");
                    }
                }
                writer.BreakLine();
            }
            writer.WriteLine(2, "return true;");
            writer.WriteLine(1, "}");

            writer.BreakLine();
            writer.WriteLine(1, "ModelFactory::Registry<{0}> {0}::_registry;", md->name());
            writer.BreakLine();
        }

        writer.WriteLine(0, "}");

        return writer.GetString();
    }

    bool BehaviorTreeCppModelGenerator::IsModel(const FieldDescriptor& field)
    {
        const auto* message = field.message_type();
        if (!message)
        {
            return false;
        }

        return message->name() == "Model";
    }

    auto BehaviorTreeCppModelGenerator::ToCppType(const FieldDescriptor& field) -> std::string
    {
        // todo: optional
        if (field.message_type())
        {
            return field.message_type()->name();
        }

        std::string protoTypeName(field.cpp_type_name());

        if (protoTypeName.starts_with("int"))
        {
            return std::format("{}_t", field.cpp_type_name());
        }

        return protoTypeName;
    }
}
