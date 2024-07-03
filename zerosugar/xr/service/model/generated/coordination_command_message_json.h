#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/coordination_command_message.h"

namespace zerosugar::xr::coordination::command
{
    void from_json(const nlohmann::json& j, LaunchGameInstance& item);
    void to_json(nlohmann::json& j, const LaunchGameInstance& item);

    void from_json(const nlohmann::json& j, BroadcastChatting& item);
    void to_json(nlohmann::json& j, const BroadcastChatting& item);

    void from_json(const nlohmann::json& j, NotifyDungeonMatchGroupCreation& item);
    void to_json(nlohmann::json& j, const NotifyDungeonMatchGroupCreation& item);

    void from_json(const nlohmann::json& j, NotifyDungeonMatchGroupApproved& item);
    void to_json(nlohmann::json& j, const NotifyDungeonMatchGroupApproved& item);

    void from_json(const nlohmann::json& j, NotifyDungeonMatchGroupRejected& item);
    void to_json(nlohmann::json& j, const NotifyDungeonMatchGroupRejected& item);

}
