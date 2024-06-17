#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/network/model/generated/rpc_message.h"

namespace zerosugar::xr::network
{
    void from_json(const nlohmann::json& j, RequestRegisterRPCClient& item);
    void to_json(nlohmann::json& j, const RequestRegisterRPCClient& item);

    void from_json(const nlohmann::json& j, ResultRegisterRPCClient& item);
    void to_json(nlohmann::json& j, const ResultRegisterRPCClient& item);

    void from_json(const nlohmann::json& j, RequestRemoteProcedureCall& item);
    void to_json(nlohmann::json& j, const RequestRemoteProcedureCall& item);

    void from_json(const nlohmann::json& j, ResultRemoteProcedureCall& item);
    void to_json(nlohmann::json& j, const ResultRemoteProcedureCall& item);

    void from_json(const nlohmann::json& j, SendClientSteaming& item);
    void to_json(nlohmann::json& j, const SendClientSteaming& item);

    void from_json(const nlohmann::json& j, AbortClientStreamingRPC& item);
    void to_json(nlohmann::json& j, const AbortClientStreamingRPC& item);

}
