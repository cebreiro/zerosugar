#include "service_writer.h"

#include "zerosugar/tool/proto_code_generator/proto_code_generator_option.h"
#include "zerosugar/tool/proto_code_generator/writer/input/writer_input.h"

namespace zerosugar
{
    auto ServiceWriter::Write(const Param& param) -> std::pair<std::string, std::string>
    {
        WriteHeader(param);
        WriteCxx(param);

        return std::make_pair(_headerPrinter.Print(), _cxxPrinter.Print());
    }

    void ServiceWriter::WriteHeader(const Param& param)
    {
        const int64_t indent = 0;
        const WriterInput& input = param.input;

        _headerPrinter.AddLine(indent, "#pragma once");
        _headerPrinter.AddLine(indent, "#include <cstdint>");

        for (const std::string& include : param.includes)
        {
            _headerPrinter.AddLine(indent, "#include \"{}\"", include);
        }

        _headerPrinter.AddLine(indent, "#include \"zerosugar/shared/service/service_interface.h\"");
        _headerPrinter.AddLine(indent, "#include \"zerosugar/shared/execution/future/future.h\"");
        _headerPrinter.AddLine(indent, "#include \"zerosugar/shared/execution/channel/channel.h\"");

        _headerPrinter.BreakLine();

        const bool hasPackage = !input.package.empty();
        const int64_t classIndent = hasPackage ? indent + 1 : indent;

        _headerPrinter.AddLine(indent, "namespace zerosugar::xr { class RPCClient; }");
        _headerPrinter.BreakLine();

        if (hasPackage)
        {
            _headerPrinter.AddLine(indent, "namespace {}", input.package);
        }

        BraceGuard packageBraceGuard(_headerPrinter, hasPackage ? indent : -1, false);

        for (const Service& service : input.services)
        {
            _headerPrinter.AddLine(classIndent, "class I{} : public IService", service.name);
            std::optional<BraceGuard> serviceBraceGuard;
            serviceBraceGuard.emplace(_headerPrinter, classIndent);

            const int64_t fieldIndent = classIndent + 1;

            _headerPrinter.AddLine(classIndent, "public:");
            _headerPrinter.AddLine(fieldIndent, "static constexpr const char* name = \"{}\";", service.name);
            _headerPrinter.BreakLine();

            _headerPrinter.AddLine(classIndent, "public:");
            _headerPrinter.AddLine(fieldIndent, "virtual ~I{}() = default;", service.name);
            _headerPrinter.BreakLine();

            auto& methodSignatures = _methods[service.name];

            for (const Method& method : service.methods)
            {
                auto& [fullName, methodName, paramTypeName, returnTypeName] = methodSignatures.emplace_back();

                const std::string& str = [&]()
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

                        methodName = std::format("{}Async", method.name);

                        std::ostringstream oss;
                        oss << "auto " << methodName;

                        if (paramType.has_value())
                        {
                            oss << "(" << *paramType << " param)";
                            paramTypeName = *paramType;
                        }
                        else
                        {
                            oss << "()";
                        }

                        oss << " -> ";

                        if (returnType.has_value())
                        {
                            oss << *returnType;
                            returnTypeName = *returnType;
                        }
                        else
                        {
                            oss << "void";
                            returnTypeName = "void";
                        }

                        fullName = oss.str();
                        
                        return std::format("virtual {} = 0;", fullName);
                    }();

                _headerPrinter.AddLine(fieldIndent, str);
            }

            _headerPrinter.AddLine(fieldIndent, "auto GetName() const -> std::string_view override {{ return \"{}\"; }}", service.name);

            serviceBraceGuard.reset();
            _headerPrinter.BreakLine();

            _headerPrinter.AddLine(classIndent, "class {0}Proxy final", service.name);
            _headerPrinter.AddLine(classIndent + 1, ": public I{}", service.name);
            _headerPrinter.AddLine(classIndent + 1, ", public std::enable_shared_from_this<{}Proxy>", service.name);
            serviceBraceGuard.emplace(_headerPrinter, classIndent);

            _headerPrinter.AddLine(classIndent, "public:");
            _headerPrinter.AddLine(fieldIndent, "explicit {}Proxy(SharedPtrNotNull<RPCClient> client);", service.name);
            _headerPrinter.BreakLine();

            for (const std::string& fullName : methodSignatures | std::views::elements<0>)
            {
                _headerPrinter.AddLine(fieldIndent, "{} override;", fullName);
            }

            _cxxPrinter.BreakLine();

            _headerPrinter.AddLine(classIndent, "private:");
            _headerPrinter.AddLine(fieldIndent, "SharedPtrNotNull<RPCClient> _client;");

            serviceBraceGuard.reset();
            _headerPrinter.BreakLine();

            _headerPrinter.AddLine(classIndent, "void Configure(const SharedPtrNotNull<I{}>& service, RPCClient& rpcClient);", service.name);
        }
    }

    void ServiceWriter::WriteCxx(const Param& param)
    {
        const int64_t indent = 0;
        const WriterInput& input = param.input;

        _cxxPrinter.AddLine(indent, "#include \"{}.h\"", param.headerName);
        _cxxPrinter.BreakLine();
        _cxxPrinter.AddLine(indent, "#include \"{}.h\"", param.messageJsonFileName);
        _cxxPrinter.AddLine(indent, "#include \"zerosugar/xr/network/rpc/rpc_client.h\"");
        _cxxPrinter.BreakLine();

        const bool hasPackage = !input.package.empty();
        const int64_t methodIndent = hasPackage ? indent + 1 : indent;

        if (hasPackage)
        {
            _cxxPrinter.AddLine(indent, "namespace {}", input.package);
        }

        BraceGuard packageBraceGuard(_cxxPrinter, hasPackage ? indent : -1, false);

        for (const Service& service : input.services)
        {
            {
                _cxxPrinter.AddLine(methodIndent, "{0}Proxy::{0}Proxy(SharedPtrNotNull<RPCClient> client)", service.name);
                _cxxPrinter.AddLine(methodIndent + 1, ": _client(std::move(client))");
                BraceGuard methodBraceGuard(_cxxPrinter, methodIndent, false);
            }
            _cxxPrinter.BreakLine();

            for (const auto& [fullName, methodName, paramType, returnType] : _methods[service.name])
            {
                std::string line = fullName;

                const size_t offset = line.find_first_of(' ') + 1;
                line.insert(offset, std::format("{}Proxy::", service.name));

                _cxxPrinter.AddLine(methodIndent, "{}", line);
                {
                    BraceGuard methodBraceGuard(_cxxPrinter, methodIndent, false);

                    _cxxPrinter.AddLine(methodIndent + 1,
                        "return _client->CallRemoteProcedure<{}, {}::value_type>(name, \"{}\", param);", paramType, returnType, methodName);
                }
                _cxxPrinter.BreakLine();
            }

            {
                _cxxPrinter.AddLine(methodIndent, "void Configure(const SharedPtrNotNull<I{}>& service, RPCClient& rpcClient)", service.name);
                {
                    BraceGuard methodBraceGuard(_cxxPrinter, methodIndent, false);

                    const int64_t methodBodyIndent = methodIndent + 1;

                    for (const auto& [_, methodName, paramType, returnType] : _methods[service.name])
                    {
                        _cxxPrinter.AddLine(methodBodyIndent, "rpcClient.RegisterProcedure(\"{}\", \"{}\",", service.name, methodName);
                        _cxxPrinter.AddLine(methodBodyIndent + 1, "[service = service]({} param) -> {}", paramType, returnType);
                        _cxxPrinter.AddLine(methodBodyIndent + 1, "{");
                        _cxxPrinter.AddLine(methodBodyIndent + 2, "return service->{}(std::move(param));", methodName);
                        _cxxPrinter.AddLine(methodBodyIndent + 1, "});");
                    }
                }
                _cxxPrinter.BreakLine();
            }
        }
    }
}
