#include "code_generator_util.h"

#include <format>
#include <Windows.h>

namespace zerosugar
{
    auto GetServiceFileName(const std::string& protoFileName) -> std::string
    {
        return protoFileName.substr(0, protoFileName.size() - 6);
    }

    auto GetMessageFileName(const std::string& protoFileName) -> std::string
    {
        return protoFileName.substr(0, protoFileName.size() - 6) + "_message";
    }

    void DebugMessageBox(const std::string& str, const std::string& caption)
    {
        ::MessageBoxA(NULL, str.c_str(), caption.c_str(), 0);
    }
}
