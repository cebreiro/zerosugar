#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/database_service_message.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, AddAccountParam& item);
    void to_json(nlohmann::json& j, const AddAccountParam& item);

    void from_json(const nlohmann::json& j, AddAccountResult& item);
    void to_json(nlohmann::json& j, const AddAccountResult& item);

    void from_json(const nlohmann::json& j, GetAccountParam& item);
    void to_json(nlohmann::json& j, const GetAccountParam& item);

    void from_json(const nlohmann::json& j, GetAccountResult& item);
    void to_json(nlohmann::json& j, const GetAccountResult& item);

}
