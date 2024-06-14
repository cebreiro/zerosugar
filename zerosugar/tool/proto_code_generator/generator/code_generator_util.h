#pragma once
#include <string>

namespace zerosugar
{
    auto GetServiceFileName(const std::string& protoFileName) -> std::string;
    auto GetMessageFileName(const std::string& protoFileName) -> std::string;
    void DebugMessageBox(const std::string& str, const std::string& caption = "debug");
}
