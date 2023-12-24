#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace zerosugar::sl
{
    enum class ProtoGenerateType
    {
        Service = 1,
        StructOnly = 2,
    };

    struct ProtoCodeGeneratorOption
    {
        std::string includePath;
        ProtoGenerateType generateType;
    };

    void from_json(const nlohmann::json& j, ProtoCodeGeneratorOption& item);
}
