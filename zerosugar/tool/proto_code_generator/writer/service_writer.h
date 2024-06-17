#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "zerosugar/tool/proto_code_generator/printer.h"

namespace zerosugar
{
    struct WriterInput;

    class ServiceWriter
    {
    public:
        struct Param
        {
            const WriterInput& input;
            std::string headerName;
            std::string messageJsonFileName;
            std::vector<std::string> includes;
        };

    public:
        auto Write(const Param& param) -> std::pair<std::string, std::string>;

    private:
        void WriteHeader(const Param& param);
        void WriteCxx(const Param& param);

    private:
        Printer _headerPrinter;
        Printer _cxxPrinter;

        struct MethodContext
        {
            std::string fullName;
            std::string name;
            std::string paramTypeName;
            std::string returnTypeName;
            bool paramStream = false;
            bool returnStream = false;
        };

        std::unordered_map<std::string, std::vector<MethodContext>> _methods;
    };
}
