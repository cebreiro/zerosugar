#include "service_interface_writer.h"

#include "zerosugar/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/tool/proto_code_generator/writer/input/writer_input.h"

namespace zerosugar
{
    auto ServiceInterfaceWriter::Write(const Param& param) -> std::string
    {
        WriteContents(param);

        return _printer.Print();
    }

    void ServiceInterfaceWriter::WriteContents(const Param& param)
    {
        const int64_t indent = 0;
        const WriterInput& input = param.input;

        _printer.AddLine(indent, "#pragma once");
        _printer.AddLine(indent, "#include <cstdint>");

        for (const std::string& include : param.includes)
        {
            _printer.AddLine(indent, "#include \"{}\"", include);
        }

        _printer.AddLine(indent, "#include \"zerosugar/shared/service/service_interface.h\"");
        _printer.AddLine(indent, "#include \"zerosugar/shared/execution/future/future.h\"");
        _printer.AddLine(indent, "#include \"zerosugar/shared/execution/channel/channel.h\"");

        _printer.BreakLine();

        const bool hasPackage = !input.package.empty();
        const int64_t classIndent = hasPackage ? indent + 1 : indent;

        if (hasPackage)
        {
            _printer.AddLine(indent, "namespace {}", input.package);
        }

        BraceGuard packageBraceGuard(_printer, hasPackage ? indent : -1, false);

        for (const Service& service : input.services)
        {
            _printer.AddLine(classIndent, "class I{} : public IService", service.name);
            BraceGuard serviceBraceGuard(_printer, classIndent);
            _printer.AddLine(classIndent, "public:");

            const int64_t fieldIndent = classIndent + 1;

            _printer.AddLine(fieldIndent, "virtual ~I{}() = default;", service.name);
            _printer.BreakLine();

            for (const Method& method : service.methods)
            {
                const std::optional<std::string> paramType = [](const Method::inout_type& type) -> std::optional<std::string>
                    {
                        if (!type.has_value())
                        {
                            return std::nullopt;
                        }

                        const bool streaming = type->second;

                        return streaming ? std::format("SharedPtrNotNull<Channel<{}>>", type->first) : type->first;
                    }(method.input);
                const std::optional<std::string> returnType = [](const Method::inout_type& type) -> std::optional<std::string>
                    {
                        if (!type.has_value())
                        {
                            return std::nullopt;
                        }

                        const bool streaming = type->second;
                        if (streaming)
                        {
                            return std::format("SharedPtrNotNull<Channel<{}>>", type->first);
                        }

                        return std::format("Future<{}>", type->first);
                    }(method.output);

                std::ostringstream oss;
                oss << "virtual auto " << method.name << "Async";

                if (paramType.has_value())
                {
                    oss << "(" << *paramType << " param)";
                }
                else
                {
                    oss << "()";
                }

                oss << " -> ";

                if (returnType.has_value())
                {
                    oss << *returnType;
                }
                else
                {
                    oss << "void";
                }

                oss << " = 0;";

                _printer.AddLine(fieldIndent, oss.str());
            }
        }
    }
}
