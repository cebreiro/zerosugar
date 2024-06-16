#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/game_service_message.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, GetNameParam& item);
    void to_json(nlohmann::json& j, const GetNameParam& item);

    void from_json(const nlohmann::json& j, GetNameResult& item);
    void to_json(nlohmann::json& j, const GetNameResult& item);

    void from_json(const nlohmann::json& j, RequestSnowflakeKeyParam& item);
    void to_json(nlohmann::json& j, const RequestSnowflakeKeyParam& item);

    void from_json(const nlohmann::json& j, RequestSnowflakeKeyResult& item);
    void to_json(nlohmann::json& j, const RequestSnowflakeKeyResult& item);

    void from_json(const nlohmann::json& j, ReturnSnowflakeKeyParam& item);
    void to_json(nlohmann::json& j, const ReturnSnowflakeKeyParam& item);

    void from_json(const nlohmann::json& j, ReturnSnowflakeKeyResult& item);
    void to_json(nlohmann::json& j, const ReturnSnowflakeKeyResult& item);

}
