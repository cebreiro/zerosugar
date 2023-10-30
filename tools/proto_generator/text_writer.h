#pragma once
#include <string>
#include <sstream>
#include <format>
#include "tools/proto_generator/constants.h"

namespace zerosugar
{
    class TextWriter
    {
    public:
        TextWriter(const TextWriter& other) = delete;
        TextWriter(TextWriter&& other) noexcept = delete;
        TextWriter& operator=(const TextWriter& other) = delete;
        TextWriter& operator=(TextWriter&& other) noexcept = delete;

        TextWriter() = default;
        ~TextWriter() = default;

        template <typename... Args>
        auto WriteLine(int64_t indent, const std::string& str, Args&&... args) -> TextWriter&;

        auto BreakLine() -> TextWriter&;

        auto GetString() const -> std::string;

    private:
        std::ostringstream _oss;
    };

    template <typename ... Args>
    auto TextWriter::WriteLine(int64_t indent, const std::string& str, Args&&... args) -> TextWriter&
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

        _oss << oss.str();

        return *this;
    }
}
