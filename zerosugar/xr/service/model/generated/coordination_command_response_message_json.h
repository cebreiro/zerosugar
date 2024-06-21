#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/coordination_command_response_message.h"

namespace zerosugar::xr::coordination::command::response
{
    void from_json(const nlohmann::json& j, Exception& item);
    void to_json(nlohmann::json& j, const Exception& item);

    void from_json(const nlohmann::json& j, Authenticate& item);
    void to_json(nlohmann::json& j, const Authenticate& item);

    void from_json(const nlohmann::json& j, LaunchGameInstance& item);
    void to_json(nlohmann::json& j, const LaunchGameInstance& item);

}
