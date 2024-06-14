#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/login_service_message.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, LoginParam& item);
    void to_json(nlohmann::json& j, const LoginParam& item);

    void from_json(const nlohmann::json& j, LoginResult& item);
    void to_json(nlohmann::json& j, const LoginResult& item);

    void from_json(const nlohmann::json& j, CreateAccountParam& item);
    void to_json(nlohmann::json& j, const CreateAccountParam& item);

    void from_json(const nlohmann::json& j, CreateAccountResult& item);
    void to_json(nlohmann::json& j, const CreateAccountResult& item);

}
