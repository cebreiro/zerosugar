#pragma once
#include <vector>
#include <string>
#include <functional>
#include "zerosugar/shared/code_gen/printer.h"

namespace zerosugar::sl
{
    struct WriterInput;
    struct Message;
    struct Field;

    class SlMessageWriter
    {
    public:
        struct Param
        {
            const WriterInput& input;
            std::string headerName;
            std::vector<std::string> includes;
        };

    public:
        bool CanWrite(const Param& param) const;
        auto Write(const Param& param) -> std::pair<std::string, std::string>;

    private:
        void WriteHeader(const Param& param);
        void WriteCxx(const Param& param);

    private:
        auto ResolveType(const Field& field) -> std::string;
        static auto MessageFilter() -> std::function<bool(const Message&)>;

    private:
        Printer _headerPrinter;
        Printer _cxxPrinter;
    };
}
