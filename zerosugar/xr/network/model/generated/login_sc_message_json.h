#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/network/model/generated/login_sc_message.h"

namespace zerosugar::xr::network::login::sc
{
    void from_json(const nlohmann::json& j, CreateAccountResult& item);
    void to_json(nlohmann::json& j, const CreateAccountResult& item);

    void from_json(const nlohmann::json& j, LoginResult& item);
    void to_json(nlohmann::json& j, const LoginResult& item);

}
