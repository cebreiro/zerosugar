#include "xr_packet_writer.h"

#include "zerosugar/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/tool/proto_code_generator/writer/input/writer_input.h"

namespace zerosugar
{
    bool XRPacketWriter::CanWrite(const Param& param) const
    {
        auto range = param.input.messages | std::views::filter(MessageFilter());
        return range.begin() != range.end();
    }

    auto XRPacketWriter::Write(const Param& param) -> std::pair<std::string, std::string>
    {
        WriteHeader(param);
        WriteCxx(param);

        return std::make_pair(_headerPrinter.Print(), _cxxPrinter.Print());
    }

    void XRPacketWriter::WriteHeader(const Param& param)
    {
        const int64_t indent = 0;
        const WriterInput& input = param.input;

        _headerPrinter.AddLine(indent, "#pragma once");
        _headerPrinter.AddLine(indent, "#include <cstdint>");
        _headerPrinter.AddLine(indent, "#include <string>");
        _headerPrinter.AddLine(indent, "#include <vector>");
        _headerPrinter.AddLine(indent, "#include \"zerosugar/xr/network/packet_deserializable.h\"");
        _headerPrinter.AddLine(indent, "#include \"zerosugar/xr/network/packet_serializable.h\"");
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

            _headerPrinter.AddLine(messageIndent, "struct {} : IPacketDeserializable, IPacketSerializable", message.name);

            std::optional<BraceGuard> messageBraceGuard;
            messageBraceGuard.emplace(_headerPrinter, messageIndent);

            const int64_t fieldIndent = messageIndent + 1;

            _headerPrinter.AddLine(fieldIndent, "static constexpr int32_t opcode = {};", *message.option.opcode);
            _headerPrinter.BreakLine();
            _headerPrinter.AddLine(fieldIndent, "void Deserialize(PacketReader& reader) final;");
            _headerPrinter.AddLine(fieldIndent, "void Serialize(PacketWriter& writer) const final;");

            if (!message.fields.empty())
            {
                _headerPrinter.BreakLine();
            }

            for (const Field& field : message.fields)
            {
                _headerPrinter.AddLine(fieldIndent, "{} {} = {{}};", ResolveType(field), field.name);
            }

            messageBraceGuard.reset();
            _headerPrinter.BreakLine();
        }
    }

    void XRPacketWriter::WriteCxx(const Param& param)
    {
        const int64_t indent = 0;
        const WriterInput& input = param.input;

        _cxxPrinter.AddLine(indent, "#include \"{}.h\"", param.headerName);
        _cxxPrinter.BreakLine();
        _cxxPrinter.AddLine(indent, "#include \"zerosugar/xr/network/packet_reader.h\"");
        _cxxPrinter.AddLine(indent, "#include \"zerosugar/xr/network/packet_writer.h\"");
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
                _cxxPrinter.AddLine(methodIndent, "void {}::Deserialize(PacketReader& reader)", message.name);
                BraceGuard methodBraceGuard(_cxxPrinter, methodIndent, false);

                const int64_t fieldIndent = methodIndent + 1;

                _cxxPrinter.AddLine(fieldIndent, "if (reader.Read<int32_t>() != {}::opcode)", message.name);
                {
                    BraceGuard ifBraceGuard(_cxxPrinter, fieldIndent, false);
                    _cxxPrinter.AddLine(fieldIndent + 1, "throw std::runtime_error(\"{} invalid opcode\");", message.name);
                }
                _cxxPrinter.BreakLine();

                for (const Field& field : message.fields)
                {
                    const std::string& type = ResolveType(field);

                    if (type.starts_with("int") || type.starts_with("float") || type.starts_with("bool"))
                    {
                        _cxxPrinter.AddLine(fieldIndent, "{} = reader.Read<{}>();", field.name, type);
                    }
                    else if (type.starts_with("std::string"))
                    {
                        _cxxPrinter.AddLine(fieldIndent, "{} = reader.ReadString();", field.name);
                    }
                    else if (type.starts_with("std::vector"))
                    {
                        if (!field.option.sizeElement.has_value())
                        {
                            throw std::runtime_error(std::format("field option-size element- not found. field: {}", field.name));
                        }

                        _cxxPrinter.AddLine(fieldIndent, "for (int32_t i = 0; i < {}; ++i)", *field.option.sizeElement);
                        BraceGuard loopBraceGuard(_cxxPrinter, fieldIndent, false);

                        _cxxPrinter.AddLine(fieldIndent + 1, "{}.emplace_back(reader.Read<{}>());", field.name, GetValueType(type));
                    }
                    else
                    {
                        _cxxPrinter.AddLine(fieldIndent, "{} = reader.Read<{}>();", field.name, type);
                    }
                }
            }
            _cxxPrinter.BreakLine();

            // Serialize
            {
                _cxxPrinter.AddLine(methodIndent, "void {}::Serialize(PacketWriter& writer) const", message.name);
                BraceGuard methodBraceGuard(_cxxPrinter, methodIndent, false);

                const int64_t fieldIndent = methodIndent + 1;

                _cxxPrinter.AddLine(fieldIndent, "writer.Write<int32_t>({}::opcode);", message.name);

                for (const Field& field : message.fields)
                {
                    const std::string& type = ResolveType(field);

                    if (type.starts_with("int") || type.starts_with("float") || type.starts_with("bool"))
                    {
                        _cxxPrinter.AddLine(fieldIndent, "writer.Write<{1}>({0});", field.name, type);
                    }
                    else if (type.starts_with("std::string"))
                    {
                        _cxxPrinter.AddLine(fieldIndent, "writer.Write({});", field.name);
                    }
                    else if (type.starts_with("std::vector"))
                    {
                        _cxxPrinter.AddLine(fieldIndent, "for (const auto& item : {})", field.name);
                        BraceGuard loopBraceGuard(_cxxPrinter, fieldIndent, false);

                        _cxxPrinter.AddLine(fieldIndent + 1, "writer.WriteObject(item);");
                    }
                    else
                    {
                        _cxxPrinter.AddLine(fieldIndent, "writer.WriteObject({});", field.name);
                    }
                }
            }
            _cxxPrinter.BreakLine();
        }
    }

    auto XRPacketWriter::GetValueType(const std::string& type) -> std::string
    {
        auto begin = type.find_first_of('<');
        auto end = type.find_last_of('>');

        return type.substr(begin + 1, end - begin - 1);
    }

    auto XRPacketWriter::ResolveType(const Field& field) -> std::string
    {
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

    auto XRPacketWriter::MessageFilter() -> std::function<bool(const Message&)>
    {
        return [](const Message& message)
            {
                return message.option.opcode.has_value();
            };
    }
}
