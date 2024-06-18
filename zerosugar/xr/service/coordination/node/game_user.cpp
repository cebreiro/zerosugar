#include "game_user.h"

namespace zerosugar::xr::coordination
{
    GameUser::GameUser(game_user_id_type id, std::string authToken, int64_t accountId, int64_t characterId, int32_t zoneId)
        : _id(id)
        , _authToken(std::move(authToken))
        , _accountId(accountId)
        , _characterId(characterId)
        , _zoneId(zoneId)
    {
    }

    auto GameUser::GetAuthToken() const -> std::string_view
    {
        return _authToken;
    }

    auto GameUser::GetAccountId() const -> int64_t
    {
        return _accountId;
    }

    auto GameUser::GetCharacterId() const -> int64_t
    {
        return _characterId;
    }

    auto GameUser::GetZoneId() const -> int32_t
    {
        return _zoneId;
    }

    auto GameUser::GetId() const -> const game_user_id_type&
    {
        return _id;
    }
}
