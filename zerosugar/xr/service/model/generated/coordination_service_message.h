#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

namespace zerosugar::xr::service
{
    enum class CoordinationServiceErrorCode : int64_t
    {
        CoordinationErrorNone = 0,
        CoordinationErrorInternalError = 1,
        CoordinationErrorFailAuthentication = 2,
        RegisterErrorDuplicatedAddress = 10001,
        RequestSnowflakeKeyErrorOutOfPool = 30001,
        ReturnSnowflakeKeyErrorInvalidKey = 40001,
        RemovePlayerErrorInvalidServer = 50001,
        RemovePlayerErrorUserNotFound = 50002,
        AuthenticatePlayerErrorUserNotFound = 60001,
        AuthenticatePlayerErrorUserIsNotMigrating = 60002,
        AuthenticatePlayerErrorRequestToInvalidServer = 60003,
        RemoveGameInstanceErrorInstanceNotFound = 70001,
        RemoveGameInstanceErrorInstanceNotEmpty = 70002,

    };
    auto GetEnumName(CoordinationServiceErrorCode e) -> std::string_view;

    struct RegisterServerParam
    {
        std::string name = {};
        std::string ip = {};
        int32_t port = {};
    };

    struct RegisterServerResult
    {
        CoordinationServiceErrorCode errorCode = {};
        int64_t serverId = {};
    };

    struct UpdateServerStatusParam
    {
        int64_t serverId = {};
        int32_t loadCPUPercentage = {};
        double freePhysicalMemoryGB = {};
    };

    struct UpdateServerStatusResult
    {
        CoordinationServiceErrorCode errorCode = {};
    };

    struct CoordinationCommandResponse
    {
        int64_t responseId = {};
        int32_t opcode = {};
        std::string contents = {};
    };

    struct CoordinationCommand
    {
        std::optional<int64_t> responseId = {};
        int32_t opcode = {};
        std::string contents = {};
    };

    struct RequestSnowflakeKeyParam
    {
        std::string requester = {};
    };

    struct RequestSnowflakeKeyResult
    {
        CoordinationServiceErrorCode errorCode = {};
        int32_t snowflakeKey = {};
    };

    struct ReturnSnowflakeKeyParam
    {
        std::string requester = {};
        int32_t snowflakeKey = {};
    };

    struct ReturnSnowflakeKeyResult
    {
        CoordinationServiceErrorCode errorCode = {};
    };

    struct AddPlayerParam
    {
        std::string authenticationToken = {};
        int64_t accountId = {};
        int64_t characterId = {};
        int32_t zoneId = {};
    };

    struct AddPlayerResult
    {
        CoordinationServiceErrorCode errorCode = {};
        std::string ip = {};
        int32_t port = {};
    };

    struct RemovePlayerParam
    {
        int64_t serverId = {};
        std::string authenticationToken = {};
    };

    struct RemovePlayerResult
    {
        CoordinationServiceErrorCode errorCode = {};
    };

    struct AuthenticatePlayerParam
    {
        int64_t serverId = {};
        std::string authenticationToken = {};
    };

    struct AuthenticatePlayerResult
    {
        CoordinationServiceErrorCode errorCode = {};
        int64_t accountId = {};
        int64_t characterId = {};
        int64_t gameInstanceId = {};
        int64_t userUniqueId = {};
    };

    struct RemoveGameInstanceParam
    {
        int64_t gameInstanceId = {};
    };

    struct RemoveGameInstanceResult
    {
        CoordinationServiceErrorCode errorCode = {};
    };

    struct BroadcastChattingParam
    {
        int64_t serverId = {};
        int64_t gameInstanceId = {};
        std::string authenticationToken = {};
        std::string message = {};
    };

    struct BroadcastChattingResult
    {
        CoordinationServiceErrorCode errorCode = {};
    };

    struct RequestDungeonMatchParam
    {
        int64_t serverId = {};
        int64_t gameInstanceId = {};
        std::string authenticationToken = {};
        int32_t dungeonId = {};
    };

    struct RequestDungeonMatchResult
    {
        CoordinationServiceErrorCode errorCode = {};
    };

    struct CancelDungeonMatchParam
    {
        int64_t serverId = {};
        int64_t gameInstanceId = {};
        std::string authenticationToken = {};
    };

    struct CancelDungeonMatchResult
    {
        CoordinationServiceErrorCode errorCode = {};
    };

    struct ApproveDungeonMatchParam
    {
        int64_t serverId = {};
        int64_t gameInstanceId = {};
        std::string authenticationToken = {};
    };

    struct ApproveDungeonMatchResult
    {
        CoordinationServiceErrorCode errorCode = {};
    };

    struct RejectDungeonMatchParam
    {
        int64_t serverId = {};
        int64_t gameInstanceId = {};
        std::string authenticationToken = {};
    };

    struct RejectDungeonMatchResult
    {
        CoordinationServiceErrorCode errorCode = {};
    };

}
