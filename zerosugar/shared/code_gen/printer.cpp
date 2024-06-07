#include "printer.h"

namespace zerosugar
{
    void Printer::BreakLine()
    {
        _oss << line_feed;
    }

    auto Printer::Print() -> std::string
    {
        return _oss.str();
    }

    BraceGuard::BraceGuard(Printer& printer, int64_t indent, bool appendSemicolon)
        : _printer(printer)
        , _indent(indent)
        , _appendSemicolon(appendSemicolon)
    {
        if (_indent >= 0)
        {
            _printer.AddLine(_indent, "{");
        }
    }

    BraceGuard::~BraceGuard()
    {
        if (_indent >= 0)
        {
            if (_appendSemicolon)
            {
                _printer.AddLine(_indent, "};");
            }
            else
            {
                _printer.AddLine(_indent, "}");
            }
        }
    }
}
