#include "game_view_controller.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/controller/game_entity_controller_interface.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/view/game_player_view_model.h"
#include "zerosugar/xr/server/game/instance/view/game_view_model_container.h"
#include "zerosugar/xr/server/game/instance/view/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/view/grid/game_spatial_sector.h"
#include "zerosugar/xr/server/game/packet/packet_builder.h"

namespace zerosugar::xr
{
    GameViewController::GameViewController(GameInstance& instance)
        : _gameInstance(instance)
    {
    }

    void GameViewController::ProcessMovement(game_entity_id_type id, const Eigen::Vector3d& position)
    {
        GameViewModelContainer& viewModelContainer = _gameInstance.GetEntityViewModelContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();

        GamePlayerViewModel* viewModel = viewModelContainer.FindPlayer(id);
        if (!viewModel)
        {
            return;
        }

        const Eigen::Vector3d oldPosition = viewModel->GetPosition();
        viewModel->SetPosition(position);

        GameSpatialSector& oldSector = spatialContainer.GetSector(oldPosition.x(), oldPosition.y());
        GameSpatialSector& newSector = spatialContainer.GetSector(position.x(), position.y());

        if (oldSector.GetId() == newSector.GetId())
        {
            if (!oldSector.HasEntitiesAtLeast(GameEntityType::Player, 1))
            {
                return;
            }

            network::game::sc::MoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, *viewModel);

            Broadcast(packet, oldSector, id);

            return;
        }

        oldSector.RemoveEntity(id);

        if (GameSpatialSector::Subset outs = (oldSector - newSector);
            !outs.Empty() && outs.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::RemoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, *viewModel);

            Broadcast(packet, outs, GameEntityType::Player);
        }

        if (GameSpatialSector::Subset unchanged = (newSector & oldSector);
            !unchanged.Empty() && unchanged.HasEntitiesAtLeast(GameEntityType::Player, 1))
        {
            network::game::sc::MoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, *viewModel);

            Broadcast(packet, unchanged, GameEntityType::Player, id);
        }

        if (GameSpatialSector::Subset ins = (newSector - oldSector);
            !ins.Empty() && ins.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::AddRemotePlayer packet;
            GamePacketBuilder::Build(packet, *viewModel);

            Broadcast(packet, ins, GameEntityType::Player);
        }

        newSector.AddEntity(id);

        ZEROSUGAR_LOG_DEBUG(_gameInstance.GetServiceLocator(),
            std::format("movement: old[{}, {}] new[{}, [{}], old_sector[{}, {}], new_sector[{}, {}]",
                oldPosition.x(), oldPosition.y(),
                position.x(), position.y(),
                oldSector.GetId().GetX(), oldSector.GetId().GetY(),
                newSector.GetId().GetX(), newSector.GetId().GetY()));
    }

    template <typename T>
    void GameViewController::Broadcast(const T& packet, const detail::game::GameSpatialSet& set, std::optional<game_entity_id_type> excluded)
    {
        Broadcast(packet, T::opcode, set, excluded);
    }

    template <typename T>
    void GameViewController::Broadcast(const T& packet, const detail::game::GameSpatialSet& set, GameEntityType type, std::optional<game_entity_id_type> excluded)
    {
        Broadcast(packet, T::opcode, set, type, excluded);
    }

    void GameViewController::Broadcast(const IPacket& packet, int32_t opcode, const detail::game::GameSpatialSet& set, std::optional<game_entity_id_type> excluded)
    {
        GameViewModelContainer& viewModelContainer = _gameInstance.GetEntityViewModelContainer();

        for (const game_entity_id_type id : set.GetEntities())
        {
            if (excluded.has_value() && *excluded == id)
            {
                continue;
            }

            GamePlayerViewModel* viewModel = viewModelContainer.FindPlayer(id);
            assert(viewModel);

            Send(packet, opcode, viewModel->GetController());
        }
    }

    void GameViewController::Broadcast(const IPacket& packet, int32_t opcode, const detail::game::GameSpatialSet& set, GameEntityType type, std::optional<game_entity_id_type> excluded)
    {
        GameViewModelContainer& viewModelContainer = _gameInstance.GetEntityViewModelContainer();

        for (const game_entity_id_type id : set.GetEntities(type))
        {
            if (excluded.has_value() && *excluded == id)
            {
                continue;
            }

            GamePlayerViewModel* viewModel = viewModelContainer.FindPlayer(id);
            assert(viewModel);

            Send(packet, opcode, viewModel->GetController());
        }
    }

    template <typename T>
    void GameViewController::Send(const T& packet, IGameController& controller)
    {
        Send(packet, T::opcode, controller);
    }

    void GameViewController::Send(const IPacket& packet, int32_t opcode, IGameController& controller)
    {
        if (controller.IsSubscriberOf(opcode))
        {
            controller.Notify(packet);
        }
    }
}
