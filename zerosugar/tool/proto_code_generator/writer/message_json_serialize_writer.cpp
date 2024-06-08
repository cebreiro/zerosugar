#include "message_json_serialize_writer.h"

#include "zerosugar/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/tool/proto_code_generator/writer/input/writer_input.h"

namespace zerosugar::sl
{
    auto MessageJsonSerializeWriter::Write(const Param& param) -> std::pair<std::string, std::string>
    {
        WriteHeader(param);
        WriteCxx(param);

        return std::make_pair(_headerPrinter.Print(), _cxxPrinter.Print());
    }

    void MessageJsonSerializeWriter::WriteHeader(const Param& param)
    {
        const int64_t indent = 0;
        const WriterInput& input = param.input;

        _headerPrinter.AddLine(indent, "#pragma once");
        _headerPrinter.AddLine(indent, "#include <cstdint>");
        _headerPrinter.AddLine(indent, "#include <string>");
        _headerPrinter.AddLine(indent, "#include <optional>");
        _headerPrinter.AddLine(indent, "#include <vector>");
        _headerPrinter.AddLine(indent, "#include <map>");
        _headerPrinter.AddLine(indent, "#include <nlohmann/json.hpp>");

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

        for (const Message& message : input.messages)
        {
            const int64_t messageIndent = classIndent;

            _headerPrinter.AddLine(messageIndent, "void from_json(const nlohmann::json& j, {}& item);", message.name);
            _headerPrinter.AddLine(messageIndent, "void to_json(nlohmann::json& j, const {}& item);", message.name);
            _headerPrinter.BreakLine();
        }
    }

    void MessageJsonSerializeWriter::WriteCxx(const Param& param)
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

        for (const Message& message : input.messages)
        {
            const int64_t messageIndent = classIndent;

            {
                _cxxPrinter.AddLine(messageIndent, "void from_json(const nlohmann::json& j, {}& item)", message.name);
                BraceGuard braceGuard(_cxxPrinter, messageIndent, false);

                const int64_t fieldIndent = messageIndent + 1;

                if (message.fields.empty())
                {
                    _cxxPrinter.AddLine(fieldIndent, "(void)j;");
                    _cxxPrinter.AddLine(fieldIndent, "(void)item;");
                }
                else
                {
                    for (const Field& field : message.fields)
                    {
                        if (field.optional)
                        {
                            _cxxPrinter.AddLine(fieldIndent, "if (const auto iter = j.find(\"{}\"); iter != j.end())", field.name);
                            BraceGuard ifBraceGuard(_cxxPrinter, fieldIndent, false);

                            _cxxPrinter.AddLine(fieldIndent + 1, "item.{}.emplace(*iter);", field.name);
                        }
                        else
                        {
                            _cxxPrinter.AddLine(fieldIndent, "j.at(\"{0}\").get_to(item.{0});", field.name);
                        }
                    }
                }
            }
            _cxxPrinter.BreakLine();

            {
                _cxxPrinter.AddLine(messageIndent, "void to_json(nlohmann::json& j, const {}& item)", message.name);
                BraceGuard braceGuard(_cxxPrinter, messageIndent, false);

                const int64_t fieldIndent = messageIndent + 1;
                if (message.fields.empty())
                {
                    _cxxPrinter.AddLine(fieldIndent, "(void)j;");
                    _cxxPrinter.AddLine(fieldIndent, "(void)item;");
                }
                else
                {
                    _cxxPrinter.AddLine(fieldIndent, "j = nlohmann::json");

                    const int64_t jsonIndent = fieldIndent + 1;
                    std::optional<BraceGuard> jsonBraceGuard;
                    jsonBraceGuard.emplace(_cxxPrinter, jsonIndent);

                    bool hasOptionalField = false;

                    for (const Field& field : message.fields)
                    {
                        const int64_t jsonFieldIndent = jsonIndent + 1;

                        if (field.optional)
                        {
                            hasOptionalField = true;
                            continue;
                        }

                        _cxxPrinter.AddLine(jsonFieldIndent, "{{ \"{0}\", item.{0} }},", field.name);
                    }

                    jsonBraceGuard.reset();

                    if (hasOptionalField)
                    {
                        _cxxPrinter.BreakLine();

                        for (const Field& field : message.fields)
                        {
                            if (!field.optional)
                            {
                                continue;
                            }

                            _cxxPrinter.AddLine(fieldIndent, "if (item.{}.has_value())", field.name);
                            BraceGuard ifBraceGuard(_cxxPrinter, fieldIndent, false);

                            _cxxPrinter.AddLine(fieldIndent + 1, "j.push_back(nlohmann::json{{ \"{0}\", *item.{0} }});", field.name);
                        }
                    }
                }
            }
            _cxxPrinter.BreakLine();
        }
    }

    auto MessageJsonSerializeWriter::ResolveType(const Field& field) -> std::string
    {
        if (field.optional)
        {
            return std::format("std::optional<{}>", field.type);
        }

        if (field.map.has_value())
        {
            return std::format("std::map<{}, {}>", field.map->first, field.map->second);
        }

        if (field.repeated)
        {
            return std::format("std::vector<{}>", field.type);
        }

        return field.type;
    }
}
