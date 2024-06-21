#include "node_container.h"

#include "zerosugar/xr/service/coordination/node/game_server.h"
#include "zerosugar/xr/service/coordination/node/game_instance.h"
#include "zerosugar/xr/service/coordination/node/game_user.h"

namespace zerosugar::xr::coordination
{
    bool NodeContainer::HasServerAddress(std::string_view ip, uint16_t port) const
    {
        const auto [begin, end] = _serverAddressIndex.equal_range(MakeServerAddressKey(ip, port));

        return std::ranges::any_of(std::ranges::subrange(begin, end) | std::views::values,
            [ip, port](const GameServer* server)
            {
                return server->GetIP() == ip && server->GetPort() == port;
            });
    }

    bool NodeContainer::Add(const SharedPtrNotNull<GameServer>& server)
    {
        const auto& id = server->GetId();

        if (_servers.try_emplace(id, std::move(server)).second)
        {
            _serversForIteration.push_back(server.get());
            _serverAddressIndex.insert(std::make_pair(MakeServerAddressKey(*server), server.get()));

            return true;
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

        if (_users.try_emplace(id, user).second)
        {
            [[maybe_unused]]
            bool inserted = _userAuthTokenIndex.try_emplace(user->GetAuthToken(), user.get()).second;
            assert(inserted);

            return true;
        }

        return false;
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
        auto iter = _users.find(id);
        if (iter != _users.end())
        {
            [[maybe_unused]]
            const size_t count = _userAuthTokenIndex.erase(iter->second->GetAuthToken());
            assert(count > 0);

            _users.erase(iter);

            return true;
        }

        return false;
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

    auto NodeContainer::FindGameUser(const std::string& authToken) -> GameUser*
    {
        const auto iter = _userAuthTokenIndex.find(authToken);

        return iter != _userAuthTokenIndex.end() ? iter->second : nullptr;
    }

    auto NodeContainer::FindGameUser(const std::string& authToken) const -> const GameUser*
    {
        const auto iter = _userAuthTokenIndex.find(authToken);

        return iter != _userAuthTokenIndex.end() ? iter->second : nullptr;
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
