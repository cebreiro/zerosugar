#include "printer.h"

namespace zerosugar
{
    Printer::Printer(google::protobuf::io::Printer& impl)
        : _impl(impl)
    {
    }

    auto Printer::BreakLine() -> Printer&
    {
        _impl.Print(constants::line_break);

        return *this;
    }
}
