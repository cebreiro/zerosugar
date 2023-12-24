#pragma once
#include "zerosugar/sl/tool/proto_code_generator/printer.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/input/writer_input.h"

namespace zerosugar::sl
{
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
