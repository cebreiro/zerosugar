#pragma once
#include <vector>
#include <string>
#include "zerosugar/tool/proto_code_generator/printer.h"

namespace zerosugar
{
    struct WriterInput;
    struct Field;

    class MessageWriter
    {
    public:
        struct Param
        {
            const WriterInput& input;
            std::string headerName;
            std::vector<std::string> includes;
        };

    public:
        auto Write(const Param& param) -> std::pair<std::string, std::string>;

    private:
        void WriteHeader(const Param& param);
        void WriteCxx(const Param& param);

    private:
        static auto ResolveType(const Field& field) -> std::string;

    private:
        Printer _headerPrinter;
        Printer _cxxPrinter;
    };
}
