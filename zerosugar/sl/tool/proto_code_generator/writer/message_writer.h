#pragma once
#include "zerosugar/sl/tool/proto_code_generator/printer.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/input/writer_input.h"

namespace zerosugar::sl
{
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
        auto Write(const Param& param) -> std::string;

    private:
        void WriteHeader(const Param& param);

    private:
        static auto ResolveType(const Field& field) -> std::string;

    private:
        Printer _headerPrinter;
        Printer _cxxPrinter;
    };
}
