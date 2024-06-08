#pragma once
#include <string>
#include <sstream>
#include <format>

namespace zerosugar
{
    class Printer
    {
    public:
        static constexpr const char* line_feed = "\r\n";

    public:
        void BreakLine();

        template <typename... Args>
        void AddLine(size_t indent, const std::string& str, Args&&... args);

        auto Print() -> std::string;

    private:
        std::ostringstream _oss;
    };

    class BraceGuard
    {
    public:
        BraceGuard() = delete;

        BraceGuard(Printer& printer, int64_t indent, bool appendSemicolon = true);
        ~BraceGuard();

    private:
        Printer& _printer;
        const int64_t _indent = 0;
        bool _appendSemicolon = false;
    };

    template <typename ... Args>
    void Printer::AddLine(size_t indent, const std::string& str, Args&&... args)
    {
        if (indent == 0)
        {
            if constexpr (sizeof...(Args) == 0)
            {
                _oss << std::format("{}{}", str, line_feed);
            }
            else
            {
                auto formatStr = std::vformat(str, std::make_format_args(args...));

                _oss << std::format("{}\r\n", std::move(formatStr));
            }

            return;
        }

        std::ostringstream oss;
        for (size_t i = 0; i < indent; ++i)
        {
            oss << "    ";
        }

        if constexpr (sizeof...(Args) == 0)
        {
            _oss << std::format("{}{}{}", oss.str(), str, line_feed);
        }
        else
        {
            auto formatStr = std::vformat(str, std::make_format_args(args...));

            _oss << std::format("{}{}{}", oss.str(), std::move(formatStr), line_feed);
        }
    }
}
