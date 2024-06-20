#pragma once
#include <string>
#include <vector>

namespace zerosugar
{
    auto ExecuteCommand(const std::string& cmd) -> std::vector<std::string>;
}
