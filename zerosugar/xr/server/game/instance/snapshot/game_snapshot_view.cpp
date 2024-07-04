#include "game_snapshot_view.h"

#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_sector.h"

namespace zerosugar::xr
{
    GameSnapshotView::GameSnapshotView(GameInstance& gameInstance)
        : _gameInstance(gameInstance)
    {
    }

    GameSnapshotView::~GameSnapshotView()
    {
    }

    void GameSnapshotView::Broadcast(const IPacket& packet, std::optional<game_entity_id_type> excluded)
    {
        for (const UniquePtrNotNull<GamePlayerSnapshot>& snapshot : _gameInstance.GetSnapshotContainer().GetPlayerRange())
        {
            if (excluded.has_value() && *excluded == snapshot->GetId())
            {
                continue;
            }

            Send(packet, snapshot->GetController());
        }
    }

    void GameSnapshotView::Broadcast(const IPacket& packet, const GamePlayerSnapshot& middle, std::optional<game_entity_id_type> excluded)
    {
        GameSpatialSector& sector = _gameInstance.GetSpatialContainer().GetSector(middle.GetPosition().x(), middle.GetPosition().y());

        Broadcast(packet, sector, excluded);
    }

    void GameSnapshotView::Broadcast(const IPacket& packet, const detail::game::GameSpatialSet& set, std::optional<game_entity_id_type> excluded)
    {
        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();

        for (const game_entity_id_type id : set.GetEntities())
        {
            if (excluded.has_value() && *excluded == id)
            {
                continue;
            }

            GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
            assert(snapshot);

            Send(packet, snapshot->GetController());
        }
    }

    void GameSnapshotView::Broadcast(const IPacket& packet, const detail::game::GameSpatialSet& set, GameEntityType type, std::optional<game_entity_id_type> excluded)
    {
        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();

        for (const game_entity_id_type id : set.GetEntities(type))
        {
            if (excluded.has_value() && *excluded == id)
            {
                continue;
            }

            GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
            assert(snapshot);

            Send(packet, snapshot->GetController());
        }
    }

    void GameSnapshotView::Send(const IPacket& packet, IGameController& controller)
    {
        if (controller.IsSubscriberOf(packet.GetOpcode()))
        {
            controller.Notify(packet);
        }
    }

    void GameSnapshotView::SendDelay(std::chrono::milliseconds delay, UniquePtrNotNull<IPacket> packet, game_entity_id_type id)
    {
        auto instance = _gameInstance.shared_from_this();

        Delay(delay).Then(_gameInstance.GetStrand(),
            [instance = std::move(instance), packet = std::move(packet), id]()
            {
                GameSnapshotModelContainer& snapshotContainer = instance->GetSnapshotContainer();
                if (GamePlayerSnapshot* player = snapshotContainer.FindPlayer(id); player)
                {
                    IGameController& controller = player->GetController();
                    if (controller.IsSubscriberOf(packet->GetOpcode()))
                    {
                        controller.Notify(*packet);
                    }
                }
            });
    }
}
