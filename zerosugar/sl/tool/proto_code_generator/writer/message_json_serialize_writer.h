#pragma once
#include <vector>
#include <string>
#include "zerosugar/shared/code_gen/printer.h"

namespace zerosugar::sl
{
    struct WriterInput;
    struct Field;

    class MessageJsonSerializeWriter
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
