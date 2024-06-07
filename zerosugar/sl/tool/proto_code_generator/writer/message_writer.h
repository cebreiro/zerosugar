#pragma once
#include <vector>
#include <string>
#include "zerosugar/shared/code_gen/printer.h"

namespace zerosugar::sl
{
    struct WriterInput;
    struct Field;

    class MessageWriter
    {
    public:
        struct Param
        {
            const WriterInput& input;
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
