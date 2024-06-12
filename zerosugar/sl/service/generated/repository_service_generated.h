#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/sl/service/generated/shared_generated.h"

namespace zerosugar::sl::service
{
    enum class RepositoryServiceErrorCode : int64_t
    {
        RepositoryErrorNone = 0,
        RepositoryInternalError = 10000,
        RepositoryInternalDbError = 10001,
        RepositoryAccountError = 20000,
        RepositoryAccountFailToFindKey = 20001,
        RepositoryCharacterError = 30000,
    };
    struct FindAccountParam
    {
        std::string account = {};
    };

    struct FindAccountResult
    {
        RepositoryServiceErrorCode errorCode = {};
        std::optional<Account> account = {};
    };

    struct UpdateAccountParam
    {
        AccountUpdate accountUpdate = {};
    };

    struct UpdateAccountResult
    {
        RepositoryServiceErrorCode errorCode = {};
    };

    struct GetCharacterListParam
    {
        int64_t accountId = {};
    };

    struct GetCharacterListResult
    {
        RepositoryServiceErrorCode errorCode = {};
        std::vector<Character> character = {};
    };

    struct CreateCharacterParam
    {
        int64_t accountId = {};
        Character character = {};
    };

    struct CreateCharacterResult
    {
        RepositoryServiceErrorCode errorCode = {};
    };

    struct DeleteCharacterParam
    {
        int64_t characterId = {};
    };

    struct DeleteCharacterResult
    {
        RepositoryServiceErrorCode errorCode = {};
    };

    struct NameCheckCharacterParam
    {
        std::string name = {};
    };

    struct NameCheckCharacterResult
    {
        RepositoryServiceErrorCode errorCode = {};
    };

    struct LoadCharacterParam
    {
        int64_t characterId = {};
    };

    struct LoadCharacterResult
    {
        RepositoryServiceErrorCode errorCode = {};
        Character character = {};
    };

}
