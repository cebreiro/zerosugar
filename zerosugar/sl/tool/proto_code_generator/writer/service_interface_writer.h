#pragma once
#include <vector>
#include <string>
#include "zerosugar/sl/tool/proto_code_generator/printer.h"

namespace zerosugar::sl
{
    struct WriterInput;

    class ServiceInterfaceWriter
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
        void WriteContents(const Param& param);

    private:
        Printer _printer;
    };
}
