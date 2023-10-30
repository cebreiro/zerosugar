#include "text_writer.h"

namespace zerosugar
{
    auto TextWriter::BreakLine() -> TextWriter&
    {
        _oss << constants::line_break;

        return *this;
    }

    auto TextWriter::GetString() const -> std::string
    {
        return _oss.str();
    }
}
