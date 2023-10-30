#pragma once
#include <string>

namespace zerosugar::proto_util
{
    auto ConvertPackageToNamespace(const std::string& package) -> std::string;
}
