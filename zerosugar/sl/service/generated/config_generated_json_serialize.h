#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/sl/service/generated/config_generated.h"

namespace zerosugar::sl::service
{
    void from_json(const nlohmann::json& j, DatabaseConfig& item);
    void to_json(nlohmann::json& j, const DatabaseConfig& item);

    void from_json(const nlohmann::json& j, RepositoryServiceConfig& item);
    void to_json(nlohmann::json& j, const RepositoryServiceConfig& item);

}
