#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/gateway_service_message.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, GameServiceEndpoint& item);
    void to_json(nlohmann::json& j, const GameServiceEndpoint& item);

    void from_json(const nlohmann::json& j, AddGameServiceParam& item);
    void to_json(nlohmann::json& j, const AddGameServiceParam& item);

    void from_json(const nlohmann::json& j, AddGameServiceResult& item);
    void to_json(nlohmann::json& j, const AddGameServiceResult& item);

    void from_json(const nlohmann::json& j, GetGameServiceListParam& item);
    void to_json(nlohmann::json& j, const GetGameServiceListParam& item);

    void from_json(const nlohmann::json& j, GetGameServiceListResult& item);
    void to_json(nlohmann::json& j, const GetGameServiceListResult& item);

}
