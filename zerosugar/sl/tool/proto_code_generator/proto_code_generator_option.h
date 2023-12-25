#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace zerosugar::sl
{
    struct ProtoCodeGeneratorOption
    {
        std::string includePath;
        std::string generator;
    };

    void from_json(const nlohmann::json& j, ProtoCodeGeneratorOption& item);
}
