#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/network/model/generated/login_cs_message.h"

namespace zerosugar::xr::network::login::cs
{
    void from_json(const nlohmann::json& j, CreateAccount& item);
    void to_json(nlohmann::json& j, const CreateAccount& item);

    void from_json(const nlohmann::json& j, Login& item);
    void to_json(nlohmann::json& j, const Login& item);

}
