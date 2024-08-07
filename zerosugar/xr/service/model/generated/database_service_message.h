#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"

namespace zerosugar::xr::service
{
    enum class DatabaseServiceErrorCode : int64_t
    {
        DatabaseErrorNone = 0,
        DatabaseErrorInternalError = 1,
        AddAccountErrorDuplicated = 10001,
        GetAccountErrorNotFound = 20001,

    };
    auto GetEnumName(DatabaseServiceErrorCode e) -> std::string_view;

    struct AddAccountParam
    {
        std::string account = {};
        std::string password = {};
    };

    struct AddAccountResult
    {
        DatabaseServiceErrorCode errorCode = {};
    };

    struct GetAccountParam
    {
        std::string account = {};
    };

    struct GetAccountResult
    {
        DatabaseServiceErrorCode errorCode = {};
        DTOAccount account = {};
    };

    struct AddCharacterParam
    {
        DTOCharacterAdd characterAdd = {};
        std::vector<DTOEquipItem> equipItems = {};
        std::vector<DTOItem> items = {};
    };

    struct GetCharacterParam
    {
        int64_t characterId = {};
    };

    struct GetCharacterResult
    {
        DatabaseServiceErrorCode errorCode = {};
        DTOCharacter character = {};
    };

    struct RemoveCharacterParam
    {
        int64_t characterId = {};
    };

    struct RemoveCharacterResult
    {
        DatabaseServiceErrorCode errorCode = {};
    };

    struct AddCharacterResult
    {
        DatabaseServiceErrorCode errorCode = {};
        int64_t characterId = {};
    };

    struct CharacterItemChangeParam
    {
        int64_t characterId = {};
        std::string itemChangeLogs = {};
    };

    struct CharacterItemChangeResult
    {
        DatabaseServiceErrorCode errorCode = {};
    };

    struct GetLobbyCharactersParam
    {
        int64_t accountId = {};
    };

    struct GetLobbyCharactersResult
    {
        DatabaseServiceErrorCode errorCode = {};
        std::vector<DTOLobbyCharacter> lobbyCharacters = {};
    };

}
