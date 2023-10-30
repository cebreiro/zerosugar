#pragma once
#include <cstdint>
#include <string>
#include <format>
#include <sstream>
#include <google/protobuf/io/printer.h>
#include "tools/code_generator/constants.h"

namespace google::protobuf::io
{
    class Printer;
}

namespace zerosugar
{
    class Printer
    {
    public:
        Printer() = delete;
        Printer(const Printer& other) = delete;
        Printer(Printer&& other) noexcept = delete;
        Printer& operator=(const Printer& other) = delete;
        Printer& operator=(Printer&& other) noexcept = delete;

        explicit Printer(google::protobuf::io::Printer& impl);

        template <typename... Args>
        auto AddLine(int64_t indent, const std::string& str, Args&&... args) -> Printer&;

        auto BreakLine() -> Printer&;

    private:
        google::protobuf::io::Printer& _impl;
    };

    template <typename ... Args>
    auto Printer::AddLine(int64_t indent, const std::string& str, Args&&... args) -> Printer&
    {
        std::ostringstream oss;

        if (indent > 0)
        {
            for (int64_t i = 0; i < indent * 4; ++i)
            {
                oss << constants::space;
            }
        }

        if constexpr (sizeof...(Args) == 0)
        {
            oss << str;
        }
        else
        {
            oss << std::vformat(str, std::make_format_args(std::forward<Args>(args)...));
        }

        oss << constants::line_break;

        _impl.Print(oss.str().c_str());

        return *this;
    }
}
