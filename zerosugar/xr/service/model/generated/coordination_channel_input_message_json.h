#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/coordination_channel_input_message.h"

namespace zerosugar::xr::coordination::input
{
    void from_json(const nlohmann::json& j, Authenticate& item);
    void to_json(nlohmann::json& j, const Authenticate& item);

}
