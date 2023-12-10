#pragma once
#include "zerosugar/sl/tool/service_code_generator/printer.h"
#include "zerosugar/sl/tool/service_code_generator/writer/input/writer_input.h"

namespace zerosugar::sl
{
    class ServiceMessageWriter
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
