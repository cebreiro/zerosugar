#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/sl/service/generated/login_service_generated.h"
#include "zerosugar/sl/service/generated/shared_generated.h"
#include "zerosugar/sl/service/generated/shared_generated_json_serialize.h"

namespace zerosugar::sl::service
{
    void from_json(const nlohmann::json& j, LoginParam& item);
    void to_json(nlohmann::json& j, const LoginParam& item);

    void from_json(const nlohmann::json& j, LoginResult& item);
    void to_json(nlohmann::json& j, const LoginResult& item);

    void from_json(const nlohmann::json& j, LogoutParam& item);
    void to_json(nlohmann::json& j, const LogoutParam& item);

    void from_json(const nlohmann::json& j, LogoutResult& item);
    void to_json(nlohmann::json& j, const LogoutResult& item);

    void from_json(const nlohmann::json& j, AuthenticateParam& item);
    void to_json(nlohmann::json& j, const AuthenticateParam& item);

    void from_json(const nlohmann::json& j, AuthenticateResult& item);
    void to_json(nlohmann::json& j, const AuthenticateResult& item);

    void from_json(const nlohmann::json& j, AuthorizeParam& item);
    void to_json(nlohmann::json& j, const AuthorizeParam& item);

    void from_json(const nlohmann::json& j, AuthorizeResult& item);
    void to_json(nlohmann::json& j, const AuthorizeResult& item);

    void from_json(const nlohmann::json& j, KickParam& item);
    void to_json(nlohmann::json& j, const KickParam& item);

    void from_json(const nlohmann::json& j, KickResult& item);
    void to_json(nlohmann::json& j, const KickResult& item);

}
