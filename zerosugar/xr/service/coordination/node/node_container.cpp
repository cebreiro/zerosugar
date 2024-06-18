#include "node_container.h"

#include "zerosugar/xr/service/coordination/node/game_server.h"
#include "zerosugar/xr/service/coordination/node/game_instance.h"
#include "zerosugar/xr/service/coordination/node/game_user.h"

namespace zerosugar::xr::coordination
{
    bool NodeContainer::HasServerAddress(std::string_view ip, uint16_t port) const
    {
        const auto [begin, end] = _serverAddressIndex.equal_range(MakeServerAddressKey(ip, port));

        for (const GameServer* server : std::ranges::subrange(begin, end) | std::views::values)
        {
            if (server->GetIP() == ip && server->GetPort() == port)
            {
                return true;
            }
        }

        return false;
    }

    bool NodeContainer::Add(SharedPtrNotNull<GameServer> server)
    {
        const auto& id = server->GetId();

        if (_servers.try_emplace(id, std::move(server)).second)
        {
            _serversForIteration.push_back(server.get());
            _serverAddressIndex.insert(std::make_pair(MakeServerAddressKey(*server), server.get()));
        }

        return false;
    }

    bool NodeContainer::Add(SharedPtrNotNull<GameInstance> instance)
    {
        const auto& id = instance->GetId();

        return _instances.try_emplace(id, std::move(instance)).second;
    }

    bool NodeContainer::Add(SharedPtrNotNull<GameUser> user)
    {
        const auto& id = user->GetId();

        return _users.try_emplace(id, std::move(user)).second;
    }

    bool NodeContainer::Remove(game_server_id_type id)
    {
        auto iter = _servers.find(id);
        if (iter != _servers.end())
        {
            const SharedPtrNotNull<GameServer> server = std::exchange(iter->second, {});
            _servers.erase(iter);

            auto iterRange = std::ranges::find(_serversForIteration, server.get());
            assert(iterRange != _serversForIteration.end());

            _serversForIteration.erase(iterRange);

            const auto [begin, end] = _serverAddressIndex.equal_range(MakeServerAddressKey(*server));
            for (auto iterAddress = begin; iterAddress != end; ++iterAddress)
            {
                if (iterAddress->second == server.get())
                {
                    _serverAddressIndex.erase(iterAddress);

                    break;
                }
            }

            return true;
        }

        return false;
    }

    bool NodeContainer::Remove(game_instance_id_type id)
    {
        return _instances.erase(id);
    }

    bool NodeContainer::Remove(game_user_id_type id)
    {
        return _users.erase(id);
    }

    auto NodeContainer::Find(game_server_id_type id) -> GameServer*
    {
        const auto iter = _servers.find(id);

        return iter != _servers.end() ? iter->second.get() : nullptr;
    }

    auto NodeContainer::Find(game_server_id_type id) const -> const GameServer*
    {
        const auto iter = _servers.find(id);

        return iter != _servers.end() ? iter->second.get() : nullptr;
    }

    auto NodeContainer::Find(game_instance_id_type id) -> GameInstance*
    {
        const auto iter = _instances.find(id);

        return iter != _instances.end() ? iter->second.get() : nullptr;
    }

    auto NodeContainer::Find(game_instance_id_type id) const -> const GameInstance*
    {
        const auto iter = _instances.find(id);

        return iter != _instances.end() ? iter->second.get() : nullptr;
    }

    auto NodeContainer::Find(game_user_id_type id) -> GameUser*
    {
        const auto iter = _users.find(id);

        return iter != _users.end() ? iter->second.get() : nullptr;
    }

    auto NodeContainer::Find(game_user_id_type id) const -> const GameUser*
    {
        const auto iter = _users.find(id);

        return iter != _users.end() ? iter->second.get() : nullptr;
    }

    auto NodeContainer::MakeServerAddressKey(const GameServer& server) -> int64_t
    {
        return MakeServerAddressKey(server.GetIP(), server.GetPort());
    }

    auto NodeContainer::MakeServerAddressKey(std::string_view ip, uint16_t port) -> int64_t
    {
        int64_t result = port;

        for (const char c : ip)
        {
            result += c;
        }

        return result;
    }
}
