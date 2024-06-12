#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/sl/service/generated/world_service_generated.h"
#include "zerosugar/sl/service/generated/shared_generated.h"
#include "zerosugar/sl/service/generated/shared_generated_json_serialize.h"

namespace zerosugar::sl::service
{
    void from_json(const nlohmann::json& j, World& item);
    void to_json(nlohmann::json& j, const World& item);

    void from_json(const nlohmann::json& j, CreateWorldParam& item);
    void to_json(nlohmann::json& j, const CreateWorldParam& item);

    void from_json(const nlohmann::json& j, CreateWorldResult& item);
    void to_json(nlohmann::json& j, const CreateWorldResult& item);

    void from_json(const nlohmann::json& j, CreateZoneParam& item);
    void to_json(nlohmann::json& j, const CreateZoneParam& item);

    void from_json(const nlohmann::json& j, CreateZoneResult& item);
    void to_json(nlohmann::json& j, const CreateZoneResult& item);

    void from_json(const nlohmann::json& j, GetWorldListParam& item);
    void to_json(nlohmann::json& j, const GetWorldListParam& item);

    void from_json(const nlohmann::json& j, GetWorldListResult& item);
    void to_json(nlohmann::json& j, const GetWorldListResult& item);

    void from_json(const nlohmann::json& j, EnterWorldParam& item);
    void to_json(nlohmann::json& j, const EnterWorldParam& item);

    void from_json(const nlohmann::json& j, EnterWorldResult& item);
    void to_json(nlohmann::json& j, const EnterWorldResult& item);

    void from_json(const nlohmann::json& j, KickPlayerParam& item);
    void to_json(nlohmann::json& j, const KickPlayerParam& item);

    void from_json(const nlohmann::json& j, KickPlayerResult& item);
    void to_json(nlohmann::json& j, const KickPlayerResult& item);

}
