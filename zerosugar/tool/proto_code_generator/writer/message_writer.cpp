#include "message_writer.h"

#include "zerosugar/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/tool/proto_code_generator/writer/input/writer_input.h"

namespace zerosugar::sl
{
    auto MessageWriter::Write(const Param& param) -> std::string
    {
        WriteHeader(param);

        return _headerPrinter.Print();
    }

    void MessageWriter::WriteHeader(const Param& param)
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

        for (const Enum& e : input.enums)
        {
            const int64_t enumIndent = classIndent;

            _headerPrinter.AddLine(enumIndent, "enum class {} : int64_t", e.name);
            BraceGuard enumBraceGuard(_headerPrinter, enumIndent);

            for (const EnumValue& value : e.values)
            {
                const int64_t enumElementIndent = enumIndent + 1;

                _headerPrinter.AddLine(enumElementIndent, "{} = {},", value.name, value.number);
            }
        }

        for (const Message& message : input.messages)
        {
            const int64_t messageIndent = classIndent;

            _headerPrinter.AddLine(messageIndent, "struct {}", message.name);

            std::optional<BraceGuard> messageBraceGuard;
            messageBraceGuard.emplace(_headerPrinter, messageIndent);

            for (const Field& field : message.fields)
            {
                const int64_t fieldIndent = messageIndent + 1;

                _headerPrinter.AddLine(fieldIndent, "{} {} = {{}};", ResolveType(field), field.name);
            }

            messageBraceGuard.reset();
            _headerPrinter.BreakLine();
        }
    }

    auto MessageWriter::ResolveType(const Field& field) -> std::string
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
