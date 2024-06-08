#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace zerosugar
{
    struct ProtoCodeGeneratorOption
    {
        std::string includePath;
        std::string generator;
    };

    void from_json(const nlohmann::json& j, ProtoCodeGeneratorOption& item);
}
