#include "xr_message_writer.h"

#include "zerosugar/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/tool/proto_code_generator/writer/input/writer_input.h"

namespace zerosugar
{
    bool XRMessageWriter::CanWrite(const Param& param) const
    {
        auto range = param.input.messages | std::views::filter(MessageFilter());
        return range.begin() != range.end();
    }

    auto XRMessageWriter::Write(const Param& param) -> std::pair<std::string, std::string>
    {
        WriteHeader(param);
        WriteCxx(param);

        return std::make_pair(_headerPrinter.Print(), _cxxPrinter.Print());
    }

    void XRMessageWriter::WriteHeader(const Param& param)
    {
        const int64_t indent = 0;
        const WriterInput& input = param.input;

        _headerPrinter.AddLine(indent, "#pragma once");
        _headerPrinter.AddLine(indent, "#include <cstdint>");
        _headerPrinter.AddLine(indent, "#include <string>");
        _headerPrinter.AddLine(indent, "#include <vector>");
        _headerPrinter.AddLine(indent, "#include \"zerosugar/shared/network/buffer/buffer_deserializable.h\"");
        _headerPrinter.AddLine(indent, "#include \"zerosugar/shared/network/buffer/buffer_serializable.h\"");
        for (const std::string& include : param.includes)
        {
            _headerPrinter.AddLine(indent, "#include \"{}\"", include);
        }

        _headerPrinter.BreakLine();

        const bool hasPackage = !input.package.empty();
        const int64_t classIndent = hasPackage ? indent + 1 : indent;

        if (hasPackage)
        {
            _headerPrinter.AddLine(indent, "namespace {}", input.package);
        }

        BraceGuard packageBraceGuard(_headerPrinter, hasPackage ? indent : -1, false);

        for (const Enum& e : input.enums)
        {
            const int64_t enumIndent = classIndent;

            _headerPrinter.AddLine(enumIndent, "enum class {} : int32_t", e.name);
            BraceGuard enumBraceGuard(_headerPrinter, enumIndent);

            for (const EnumValue& value : e.values)
            {
                const int64_t enumElementIndent = enumIndent + 1;

                _headerPrinter.AddLine(enumElementIndent, "{} = {},", value.name, value.number);
            }
        }

        for (const Enum& e : input.enums)
        {
            _headerPrinter.AddLine(classIndent, "auto GetEnumName({} e) -> std::string_view;", e.name);
            _headerPrinter.BreakLine();
        }

        for (const Message& message : input.messages | std::views::filter(MessageFilter()))
        {
            const int64_t messageIndent = classIndent;

            _headerPrinter.AddLine(messageIndent, "struct {} : IBufferDeserializable, IBufferSerializable", message.name);

            std::optional<BraceGuard> messageBraceGuard;
            messageBraceGuard.emplace(_headerPrinter, messageIndent);

            const int64_t fieldIndent = messageIndent + 1;

            _headerPrinter.AddLine(fieldIndent, "void Deserialize(BufferReader& reader) final;");
            _headerPrinter.AddLine(fieldIndent, "void Serialize(BufferWriter& writer) const final;");
            _headerPrinter.BreakLine();

            for (const Field& field : message.fields)
            {
                _headerPrinter.AddLine(fieldIndent, "{} {} = {{}};", ResolveType(field), field.name);
            }

            messageBraceGuard.reset();
            _headerPrinter.BreakLine();
        }
    }

    void XRMessageWriter::WriteCxx(const Param& param)
    {
        const int64_t indent = 0;
        const WriterInput& input = param.input;

        _cxxPrinter.AddLine(indent, "#include \"{}.h\"", param.headerName);
        _cxxPrinter.BreakLine();

        const bool hasPackage = !input.package.empty();
        const int64_t classIndent = hasPackage ? indent + 1 : indent;

        if (hasPackage)
        {
            _cxxPrinter.AddLine(indent, "namespace {}", input.package);
        }

        BraceGuard packageBraceGuard(_cxxPrinter, hasPackage ? indent : -1, false);

        for (const Enum& e : input.enums)
        {
            _cxxPrinter.AddLine(classIndent, "auto GetEnumName({} e) -> std::string_view", e.name);
            BraceGuard functionBraceGuard(_cxxPrinter, classIndent, false);

            const int64_t innerIndent = classIndent + 1;

            _cxxPrinter.AddLine(innerIndent, "switch (e)");
            {
                BraceGuard switchBraceGuard(_cxxPrinter, innerIndent, false);

                const int64_t enumElementIndent = innerIndent + 1;

                for (const EnumValue& value : e.values)
                {
                    _cxxPrinter.AddLine(enumElementIndent, "case {0}::{1}: return \"{1}\";", e.name, value.name);
                }
            }

            _cxxPrinter.AddLine(innerIndent, "assert(false);");
            _cxxPrinter.AddLine(innerIndent, "return \"unk\";");
        }

        for (const Message& message : input.messages | std::views::filter(MessageFilter()))
        {
            const int64_t methodIndent = classIndent;

            // Deserialize
            {
                _cxxPrinter.AddLine(methodIndent, "void {}::Deserialize(BufferReader& reader)", message.name);
                BraceGuard methodBraceGuard(_cxxPrinter, methodIndent, false);

                const int64_t fieldIndent = methodIndent + 1;

                if (message.fields.empty())
                {
                    _cxxPrinter.AddLine(fieldIndent, "(void)reader;");
                }
                else
                {
                    for (const Field& field : message.fields)
                    {
                        const std::string& type = ResolveType(field);

                        if (type.starts_with("int") || type.starts_with("float") || type.starts_with("bool"))
                        {
                            _cxxPrinter.AddLine(fieldIndent, "{} = reader.Read<{}>();", field.name, type);
                        }
                        else if (type.starts_with("std::vector"))
                        {
                            _cxxPrinter.AddLine(fieldIndent, "reader.Read({}, reader.Read<int16_t>());", field.name);
                        }
                        else if (type.starts_with("std::string"))
                        {
                            _cxxPrinter.AddLine(fieldIndent, "{} = reader.ReadString(reader.Read<int16_t>());", field.name);
                        }
                        else
                        {
                            _cxxPrinter.AddLine(fieldIndent, "reader.Read({});", field.name);
                        }
                    }
                }
            }
            _cxxPrinter.BreakLine();

            // Serialize
            {
                _cxxPrinter.AddLine(methodIndent, "void {}::Serialize(BufferWriter& writer) const", message.name);
                BraceGuard methodBraceGuard(_cxxPrinter, methodIndent, false);

                const int64_t fieldIndent = methodIndent + 1;

                if (message.fields.empty())
                {
                    _cxxPrinter.AddLine(fieldIndent, "(void)writer;");
                }
                else
                {
                    for (const Field& field : message.fields)
                    {
                        const std::string& type = ResolveType(field);

                        if (type.starts_with("int") || type.starts_with("float") || type.starts_with("bool"))
                        {
                            _cxxPrinter.AddLine(fieldIndent, "writer.Write<{1}>({0});", field.name, type);
                        }
                        else if (type.starts_with("std::vector"))
                        {
                            _cxxPrinter.AddLine(fieldIndent, "writer.Write<int16_t>((int16_t)std::ssize({}) + 1);", field.name);
                            _cxxPrinter.AddLine(fieldIndent, "writer.Write({});", field.name);
                        }
                        else if (type.starts_with("std::string"))
                        {
                            _cxxPrinter.AddLine(fieldIndent, "writer.Write<int16_t>((int16_t)std::ssize({}) + 1);", field.name);
                            _cxxPrinter.AddLine(fieldIndent, "writer.WriteString({});", field.name);
                        }
                        else
                        {
                            _cxxPrinter.AddLine(fieldIndent, "writer.Write({});", field.name);
                        }
                    }
                }
            }
            _cxxPrinter.BreakLine();
        }
    }

    auto XRMessageWriter::ResolveType(const Field& field) -> std::string
    {
        if (field.type.starts_with("std::vector<char>"))
        {
            if (field.option.length.has_value())
            {
                return std::format("std::array<char, {}>", *field.option.length);
            }

            return "std::vector<char>";
        }

        if (field.repeated)
        {
            return std::format("std::vector<{}>", field.type);
        }

        if (field.map.has_value())
        {
            return std::format("std::map<{}, {}>", field.map->first, field.map->second);
        }

        if (field.type.starts_with("int") && field.option.length.has_value())
        {
            const int32_t length = *field.option.length;
            if (length == 8)
            {
                return "int8_t";
            }
            else if (length == 16)
            {
                return "int16_t";
            }
        }

        return field.type;
    }

    auto XRMessageWriter::MessageFilter() -> std::function<bool(const Message&)>
    {
        return [](const Message& message)
            {
                return !message.option.opcode.has_value();
            };
    }
}
