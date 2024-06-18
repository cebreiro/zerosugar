#pragma once
#include "zerosugar/xr/service/coordination/node/node_id.h"

namespace zerosugar::xr::coordination
{
    class GameServer;
    class GameInstance;
    class GameUser;

    class NodeContainer
    {
    public:
        bool HasServerAddress(std::string_view ip, uint16_t port) const;

        bool Add(SharedPtrNotNull<GameServer> server);
        bool Add(SharedPtrNotNull<GameInstance> instance);
        bool Add(SharedPtrNotNull<GameUser> user);

        bool Remove(game_server_id_type id);
        bool Remove(game_instance_id_type id);
        bool Remove(game_user_id_type id);

        auto Find(game_server_id_type id) -> GameServer*;
        auto Find(game_server_id_type id) const ->const GameServer*;
        auto Find(game_instance_id_type id) -> GameInstance*;
        auto Find(game_instance_id_type id) const ->const GameInstance*;
        auto Find(game_user_id_type id) -> GameUser*;
        auto Find(game_user_id_type id) const -> const GameUser*;

        inline auto GetServerRange() -> const auto&;
        inline auto GetServerRange() const -> const auto&;

        inline auto GetGameInstanceRange() const;

    private:
        static auto MakeServerAddressKey(const GameServer& server) -> int64_t;
        static auto MakeServerAddressKey(std::string_view ip, uint16_t port) -> int64_t;

    private:
        std::unordered_map<game_server_id_type, SharedPtrNotNull<GameServer>> _servers;
        std::unordered_multimap<int64_t, GameServer*> _serverAddressIndex;
        std::vector<GameServer*> _serversForIteration;

        std::unordered_map<game_instance_id_type, SharedPtrNotNull<GameInstance>> _instances;
        std::unordered_map<game_user_id_type, SharedPtrNotNull<GameUser>> _users;
    };

    auto NodeContainer::GetServerRange() -> const auto&
    {
        return _serversForIteration;
    }

    auto NodeContainer::GetServerRange() const -> const auto&
    {
        return _serversForIteration;
    }

    auto NodeContainer::GetGameInstanceRange() const
    {
        return _instances | std::views::values | std::views::transform([](const SharedPtrNotNull<GameInstance>& instance) -> GameInstance*
            {
                return instance.get();
            });
    }
}
