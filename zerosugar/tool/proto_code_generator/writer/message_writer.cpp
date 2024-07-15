#include "message_writer.h"

#include "zerosugar/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/tool/proto_code_generator/writer/input/writer_input.h"

namespace zerosugar
{
    auto MessageWriter::Write(const Param& param) -> std::pair<std::string, std::string>
    {
        WriteHeader(param);
        WriteCxx(param);

        return std::make_pair(_headerPrinter.Print(), _cxxPrinter.Print());
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

            _headerPrinter.BreakLine();
        }

        for (const Enum& e : input.enums)
        {
            _headerPrinter.AddLine(classIndent, "auto GetEnumName({} e) -> std::string_view;", e.name);
            _headerPrinter.BreakLine();
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

    void MessageWriter::WriteCxx(const Param& param)
    {
        if (param.input.enums.empty())
        {
            return;
        }

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
    }

    auto MessageWriter::ResolveType(const Field& field) -> std::string
    {
        if (field.optional)
        {
            return fmt::format("std::optional<{}>", field.type);
        }

        if (field.map.has_value())
        {
            return fmt::format("std::map<{}, {}>", field.map->first, field.map->second);
        }

        if (field.repeated)
        {
            return fmt::format("std::vector<{}>", field.type);
        }

        return field.type;
    }
}
