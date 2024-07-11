#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class IPacket;
    class IGameController;
    class GameInstance;
    class GamePlayerSnapshot;
    class GameSpatialSet;
}


namespace zerosugar::xr
{
    class GameSnapshotView
    {
    public:
        GameSnapshotView() = delete;

        explicit GameSnapshotView(GameInstance& gameInstance);
        ~GameSnapshotView();

        void DisplaySystemMessage(game_entity_id_type target, const std::string& message);

        template <typename TPacket> requires std::derived_from<TPacket, IPacket>
        bool AddObserver(const std::string& key, const std::function<void(const TPacket&)>& function);

        template <typename TPacket> requires std::derived_from<TPacket, IPacket>
        bool RemoveObserver(const std::string& key);

    public:
        void Broadcast(GameEntityType type, const IPacket& packet, const GameSpatialSet& set);

        void Broadcast(const IPacket& packet, std::optional<game_entity_id_type> excluded);
        void Broadcast(const IPacket& packet, const GamePlayerSnapshot& center, std::optional<game_entity_id_type> excluded = std::nullopt);
        void Broadcast(const IPacket& packet, const GameSpatialSet& set, std::optional<game_entity_id_type> excluded = std::nullopt);

        void Sync(IGameController& controller, const IPacket& packet);

    private:
        GameInstance& _gameInstance;

        std::unordered_map<int32_t, boost::unordered::unordered_flat_map<std::string, std::function<void(const IPacket&)>>> _observers;
    };

    template <typename TPacket> requires std::derived_from<TPacket, IPacket>
    bool GameSnapshotView::AddObserver(const std::string& key, const std::function<void(const TPacket&)>& function)
    {
        auto& observers = _observers[TPacket::opcode];

        return observers.try_emplace(key, [function](const IPacket& packet)
            {
                const TPacket* casted = packet.Cast<TPacket>();
                assert(casted);

                function(*casted);
            }).second;
    }

    template <typename TPacket> requires std::derived_from<TPacket, IPacket>
    bool GameSnapshotView::RemoveObserver(const std::string& key)
    {
        auto& observers = _observers[TPacket::opcode];

        const bool erased = observers.erase(key);

        if (observers.empty())
        {
            _observers.erase(TPacket::opcode);
        }

        return erased;
    }
}
