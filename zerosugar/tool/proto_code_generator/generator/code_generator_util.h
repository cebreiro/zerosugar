#pragma once
#include <string>

namespace zerosugar
{
    auto GetMessageFileName(const std::string& protoFileName) -> std::string;
    void DebugMessageBox(const std::string& str, const std::string& caption = "debug");
}
