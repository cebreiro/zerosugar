#include "game_snapshot_view.h"

#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_sector.h"

namespace zerosugar::xr
{
    GameSnapshotView::GameSnapshotView(GameInstance& gameInstance)
        : _gameInstance(gameInstance)
    {
    }

    GameSnapshotView::~GameSnapshotView()
    {
    }

    void GameSnapshotView::DisplaySystemMessage(game_entity_id_type target, const std::string& message)
    {
        if (target.GetType() != GameEntityType::Player)
        {
            return;
        }

        IGameController* controller = _gameInstance.GetSnapshotContainer().FindController(target);
        if (!controller)
        {
            return;
        }

        network::game::sc::NotifyChattingMessage packet;
        packet.type = static_cast<int32_t>(ChattingType::Local);
        packet.message = message;

        if (controller->IsRemoteController())
        {
            const Buffer buffer = Packet::ToBuffer(packet);

            controller->Notify(buffer);
        }
        else
        {
            controller->Notify(packet);
        }
    }

    void GameSnapshotView::Broadcast(GameEntityType type, const IPacket& packet, const GameSpatialSet& set)
    {
        if (const auto iter = _observers.find(packet.GetOpcode()); iter != _observers.end())
        {
            for (const auto& function : iter->second | std::views::values)
            {
                function(packet);
            }
        }

        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        const Buffer buffer = Packet::ToBuffer(packet);

        for (const game_entity_id_type id : set.GetEntities(type))
        {
            IGameController* controller = snapshotContainer.FindController(id);
            assert(controller);

            if (controller->IsRemoteController())
            {
                controller->Notify(buffer);
            }
            else
            {
                controller->Notify(packet);
            }
        }
    }

    void GameSnapshotView::Broadcast(const IPacket& packet, std::optional<game_entity_id_type> excluded)
    {
        if (const auto iter = _observers.find(packet.GetOpcode()); iter != _observers.end())
        {
            for (const auto& function : iter->second | std::views::values)
            {
                function(packet);
            }
        }

        const Buffer buffer = Packet::ToBuffer(packet);

        for (const UniquePtrNotNull<GamePlayerSnapshot>& snapshot : _gameInstance.GetSnapshotContainer().GetPlayerRange())
        {
            if (excluded.has_value() && *excluded == snapshot->GetId())
            {
                continue;
            }

            IGameController& controller = snapshot->GetController();

            if (controller.IsRemoteController())
            {
                controller.Notify(buffer);
            }
            else
            {
                controller.Notify(packet);
            }
        }
    }

    void GameSnapshotView::Broadcast(const IPacket& packet, const GamePlayerSnapshot& center, std::optional<game_entity_id_type> excluded)
    {
        GameSpatialSector& sector = _gameInstance.GetSpatialContainer().GetSector(center.GetPosition().x(), center.GetPosition().y());

        Broadcast(packet, sector, excluded);
    }

    void GameSnapshotView::Broadcast(const IPacket& packet, const GameSpatialSet& set, std::optional<game_entity_id_type> excluded)
    {
        if (const auto iter = _observers.find(packet.GetOpcode()); iter != _observers.end())
        {
            for (const auto& function : iter->second | std::views::values)
            {
                function(packet);
            }
        }

        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        const Buffer buffer = Packet::ToBuffer(packet);

        for (const game_entity_id_type id : set.GetEntities())
        {
            if (excluded.has_value() && *excluded == id)
            {
                continue;
            }

            IGameController* controller = snapshotContainer.FindController(id);
            assert(controller);

            if (controller->IsRemoteController())
            {
                controller->Notify(buffer);
            }
            else
            {
                controller->Notify(packet);
            }
        }
    }

    void GameSnapshotView::Sync(IGameController& controller, const IPacket& packet)
    {
        if (const auto iter = _observers.find(packet.GetOpcode()); iter != _observers.end())
        {
            for (const auto& function : iter->second | std::views::values)
            {
                function(packet);
            }
        }

        if (controller.IsRemoteController())
        {
            const Buffer buffer = Packet::ToBuffer(packet);

            controller.Notify(buffer);
        }
        else
        {
            controller.Notify(packet);
        }
    }
}
