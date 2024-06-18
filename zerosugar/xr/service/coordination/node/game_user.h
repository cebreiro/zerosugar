#pragma once
#include "zerosugar/xr/service/coordination/node/node.h"
#include "zerosugar/xr/service/coordination/node/node_id.h"

namespace zerosugar::xr::coordination
{
    class GameInstance;

    class GameUser : public Node<GameInstance*, void, void>
    {
    public:
        GameUser(game_user_id_type id, std::string authToken, int64_t accountId, int64_t characterId, int32_t zoneId);

        auto GetId() const -> const game_user_id_type&;
        auto GetAuthToken() const -> std::string_view;
        auto GetAccountId() const -> int64_t;
        auto GetCharacterId() const -> int64_t;
        auto GetZoneId() const -> int32_t;

    private:
        game_user_id_type _id;

        std::string _authToken;
        int64_t _accountId = 0;
        int64_t _characterId = 0;
        int32_t _zoneId = 0;
    };
}
