#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class IPacket;
    class IGameController;
    class GameInstance;
    class GamePlayerSnapshot;
}

namespace zerosugar::xr::detail::game
{
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

        void Broadcast(const IPacket& packet, std::optional<game_entity_id_type> excluded = std::nullopt);
        void Broadcast(const IPacket& packet, const GamePlayerSnapshot& middle, std::optional<game_entity_id_type> excluded = std::nullopt);
        void Broadcast(const IPacket& packet, const detail::game::GameSpatialSet& set, std::optional<game_entity_id_type> excluded = std::nullopt);
        void Broadcast(const IPacket& packet, const detail::game::GameSpatialSet& set, GameEntityType type, std::optional<game_entity_id_type> excluded = std::nullopt);

        void Send(const IPacket& packet, IGameController& controller);

    private:
        GameInstance& _gameInstance;
    };
}
