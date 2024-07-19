#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/coordination_service_message.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, RegisterServerParam& item);
    void to_json(nlohmann::json& j, const RegisterServerParam& item);

    void from_json(const nlohmann::json& j, RegisterServerResult& item);
    void to_json(nlohmann::json& j, const RegisterServerResult& item);

    void from_json(const nlohmann::json& j, UpdateServerStatusParam& item);
    void to_json(nlohmann::json& j, const UpdateServerStatusParam& item);

    void from_json(const nlohmann::json& j, UpdateServerStatusResult& item);
    void to_json(nlohmann::json& j, const UpdateServerStatusResult& item);

    void from_json(const nlohmann::json& j, CoordinationCommandResponse& item);
    void to_json(nlohmann::json& j, const CoordinationCommandResponse& item);

    void from_json(const nlohmann::json& j, CoordinationCommand& item);
    void to_json(nlohmann::json& j, const CoordinationCommand& item);

    void from_json(const nlohmann::json& j, RequestSnowflakeKeyParam& item);
    void to_json(nlohmann::json& j, const RequestSnowflakeKeyParam& item);

    void from_json(const nlohmann::json& j, RequestSnowflakeKeyResult& item);
    void to_json(nlohmann::json& j, const RequestSnowflakeKeyResult& item);

    void from_json(const nlohmann::json& j, ReturnSnowflakeKeyParam& item);
    void to_json(nlohmann::json& j, const ReturnSnowflakeKeyParam& item);

    void from_json(const nlohmann::json& j, ReturnSnowflakeKeyResult& item);
    void to_json(nlohmann::json& j, const ReturnSnowflakeKeyResult& item);

    void from_json(const nlohmann::json& j, AddPlayerParam& item);
    void to_json(nlohmann::json& j, const AddPlayerParam& item);

    void from_json(const nlohmann::json& j, AddPlayerResult& item);
    void to_json(nlohmann::json& j, const AddPlayerResult& item);

    void from_json(const nlohmann::json& j, RemovePlayerParam& item);
    void to_json(nlohmann::json& j, const RemovePlayerParam& item);

    void from_json(const nlohmann::json& j, RemovePlayerResult& item);
    void to_json(nlohmann::json& j, const RemovePlayerResult& item);

    void from_json(const nlohmann::json& j, AuthenticatePlayerParam& item);
    void to_json(nlohmann::json& j, const AuthenticatePlayerParam& item);

    void from_json(const nlohmann::json& j, AuthenticatePlayerResult& item);
    void to_json(nlohmann::json& j, const AuthenticatePlayerResult& item);

    void from_json(const nlohmann::json& j, RemoveGameInstanceParam& item);
    void to_json(nlohmann::json& j, const RemoveGameInstanceParam& item);

    void from_json(const nlohmann::json& j, RemoveGameInstanceResult& item);
    void to_json(nlohmann::json& j, const RemoveGameInstanceResult& item);

    void from_json(const nlohmann::json& j, BroadcastChattingParam& item);
    void to_json(nlohmann::json& j, const BroadcastChattingParam& item);

    void from_json(const nlohmann::json& j, BroadcastChattingResult& item);
    void to_json(nlohmann::json& j, const BroadcastChattingResult& item);

    void from_json(const nlohmann::json& j, RequestDungeonMatchParam& item);
    void to_json(nlohmann::json& j, const RequestDungeonMatchParam& item);

    void from_json(const nlohmann::json& j, RequestDungeonMatchResult& item);
    void to_json(nlohmann::json& j, const RequestDungeonMatchResult& item);

    void from_json(const nlohmann::json& j, CancelDungeonMatchParam& item);
    void to_json(nlohmann::json& j, const CancelDungeonMatchParam& item);

    void from_json(const nlohmann::json& j, CancelDungeonMatchResult& item);
    void to_json(nlohmann::json& j, const CancelDungeonMatchResult& item);

    void from_json(const nlohmann::json& j, ApproveDungeonMatchParam& item);
    void to_json(nlohmann::json& j, const ApproveDungeonMatchParam& item);

    void from_json(const nlohmann::json& j, ApproveDungeonMatchResult& item);
    void to_json(nlohmann::json& j, const ApproveDungeonMatchResult& item);

    void from_json(const nlohmann::json& j, RejectDungeonMatchParam& item);
    void to_json(nlohmann::json& j, const RejectDungeonMatchParam& item);

    void from_json(const nlohmann::json& j, RejectDungeonMatchResult& item);
    void to_json(nlohmann::json& j, const RejectDungeonMatchResult& item);

}
