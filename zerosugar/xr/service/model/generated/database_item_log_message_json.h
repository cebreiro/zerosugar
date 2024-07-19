#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/database_item_log_message.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, EquipItemLog& item);
    void to_json(nlohmann::json& j, const EquipItemLog& item);

    void from_json(const nlohmann::json& j, UnequipItemLog& item);
    void to_json(nlohmann::json& j, const UnequipItemLog& item);

    void from_json(const nlohmann::json& j, ShiftItemLog& item);
    void to_json(nlohmann::json& j, const ShiftItemLog& item);

    void from_json(const nlohmann::json& j, AddItemLog& item);
    void to_json(nlohmann::json& j, const AddItemLog& item);

    void from_json(const nlohmann::json& j, RemoveItemLog& item);
    void to_json(nlohmann::json& j, const RemoveItemLog& item);

    void from_json(const nlohmann::json& j, ChangeItemQuantityLog& item);
    void to_json(nlohmann::json& j, const ChangeItemQuantityLog& item);

}
