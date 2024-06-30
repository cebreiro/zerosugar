#include "packet_builder.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/entity/component/inventory_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/player_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_container.h"

namespace zerosugar::xr
{
    void GamePacketBuilder::Build(network::game::sc::EnterGame& result,
        const GameInstance& gameInstance, const GameEntity& entity, const GameSpatialSector& sector)
    {
        result.zoneId = gameInstance.GetZoneId();

        const GameSnapshotModelContainer& viewContainer = gameInstance.GetSnapshotContainer();
        {
            for (game_entity_id_type id : sector.GetEntities())
            {
                if (id == entity.GetId())
                {
                    continue;
                }

                if (const GamePlayerSnapshot* playerView = viewContainer.FindPlayer(id); playerView)
                {
                    network::game::RemotePlayer& item = result.remotePlayers.emplace_back();
                    Build(item.base, *playerView);
                    Build(item.equipment, *playerView);

                    ++result.remotePlayersCount;
                }
                else
                {
                    assert(false);
                }
            }

            result.monstersCount = 0;
        }

        Build(result.localPlayer, entity);
    }

    void GamePacketBuilder::Build(network::game::sc::AddRemotePlayer& result, const GamePlayerSnapshot& playerView)
    {
        Build(result.player, playerView);
    }

    void GamePacketBuilder::Build(network::game::sc::RemoveRemotePlayer& result, const GamePlayerSnapshot& playerView)
    {
        result.id = playerView.GetId().Unwrap();
    }

    void GamePacketBuilder::Build(network::game::sc::MoveRemotePlayer& result, const GamePlayerSnapshot& playerView)
    {
        result.id = playerView.GetId().Unwrap();
        Build(result.position, playerView);
    }

    void GamePacketBuilder::Build(network::game::Player& player, const GameEntity& entity)
    {
        player.id = entity.GetId().Unwrap();

        Build(player.base, entity);
        Build(player.equipment, entity);

        {
            auto& movementComponent = entity.GetComponent<MovementComponent>();

            network::game::Position& position = player.transform.position;

            position.x = static_cast<float>(movementComponent.GetX());
            position.y = static_cast<float>(movementComponent.GetY());
            position.z = static_cast<float>(movementComponent.GetZ());

            player.transform.rotation.yaw = static_cast<float>(movementComponent.GetYaw());
        }
        {
            auto& inventoryComponent = entity.GetComponent<InventoryComponent>();

            player.gold = 123123;

            const auto make = [](const InventoryItem& item)
                {
                    network::game::PlayerInventoryItem result;
                    result.id = item.itemDataId;
                    result.count = item.quantity;
                    result.attack = item.attack.value_or(0);
                    result.defence = item.attack.value_or(0);
                    result.str = item.attack.value_or(0);
                    result.dex = item.attack.value_or(0);
                    result.intell = item.attack.value_or(0);

                    return result;
                };

            for (const InventoryItem& item : inventoryComponent.GetInventoryItems())
            {
                player.items.emplace_back(make(item));
                ++player.itemsCount;
            }
        }
    }

    void GamePacketBuilder::Build(network::game::RemotePlayer& result, const GamePlayerSnapshot& playerView)
    {
        result.id = playerView.GetId().Unwrap();

        Build(result.base, playerView);
        Build(result.equipment, playerView);
        Build(result.transform.position, playerView);
    }

    void GamePacketBuilder::Build(network::game::PlayerBase& result, const GameEntity& entity)
    {
        auto& playerComponent = entity.GetComponent<PlayerComponent>();
        auto& statComponent = entity.GetComponent<StatComponent>();

        result.hp = 100;// statComponent.GetHP().As<float>();
        result.maxHP = 100;// statComponent.GetMaxHP().As<float>();
        result.attackMin = statComponent.Get(StatType::Attack).As<float>();
        result.attackMax = statComponent.Get(StatType::Attack).As<float>();
        result.speed = 10.f;
        result.name = playerComponent.GetName();
        result.level = playerComponent.GetLevel();
        result.gender = 0;
        result.face = playerComponent.GetFaceId();
        result.hair = playerComponent.GetHairId();
        result.str = statComponent.Get(StatType::Str).As<int32_t>();
        result.dex = statComponent.Get(StatType::Dex).As<int32_t>();
        result.intell = statComponent.Get(StatType::Intell).As<int32_t>();
        result.stamina = statComponent.GetStamina().As<float>();
        result.staminaMax = statComponent.GetMaxStamina().As<float>();
    }

    void GamePacketBuilder::Build(network::game::PlayerBase& result, const GamePlayerSnapshot& playerView)
    {
        result.hp = playerView.GetHp();
        result.maxHP = playerView.GetMaxHp();
        result.attackMin = playerView.GetAttackMin();
        result.attackMax = playerView.GetAttackMax();
        result.speed = playerView.GetSpeed();
        result.name = playerView.GetName();
        result.level = playerView.GetLevel();
        result.gender = playerView.GetGender();
        result.face = playerView.GetFace();
        result.hair = playerView.GetHair();
        result.str = playerView.GetStr();
        result.dex = playerView.GetDex();
        result.intell = playerView.GetIntell();
        result.stamina = playerView.GetStamina();
        result.staminaMax = playerView.GetStaminaMax();
    }

    void GamePacketBuilder::Build(network::game::PlayerEquipment& result, const GameEntity& entity)
    {
        auto& inventoryComponent = entity.GetComponent<InventoryComponent>();

        auto equipItems = inventoryComponent.GetEquipments();

        for (int32_t i = 0; i < static_cast<int32_t>(std::ssize(equipItems)); ++i)
        {
            const InventoryItem* item = equipItems[i];
            if (!item)
            {
                continue;
            }

            switch (static_cast<data::EquipPosition>(i))
            {
            case data::EquipPosition::Armor:
                Build(result.armor, *item, i);
                break;
            case data::EquipPosition::Gloves:
                Build(result.gloves, *item, i);
                break;
            case data::EquipPosition::Shoes:
                Build(result.shoes, *item, i);
                break;
            case data::EquipPosition::Weapon:
                Build(result.weapon, *item, i);
                break;
            case data::EquipPosition::Count:
            case data::EquipPosition::None:
                break;
            }
        }
    }

    void GamePacketBuilder::Build(network::game::PlayerEquipment& result, const GamePlayerSnapshot& playerView)
    {
        const auto& equipItems = playerView.GetEquipment();

        for (int32_t i = 0; i < static_cast<int32_t>(std::ssize(equipItems)); ++i)
        {
            const auto& item = equipItems[i];
            if (!item.has_value())
            {
                continue;
            }

            switch (static_cast<data::EquipPosition>(i))
            {
            case data::EquipPosition::Armor:
                Build(result.armor, *item, i);
                break;
            case data::EquipPosition::Gloves:
                Build(result.gloves, *item, i);
                break;
            case data::EquipPosition::Shoes:
                Build(result.shoes, *item, i);
                break;
            case data::EquipPosition::Weapon:
                Build(result.weapon , *item, i);
                break;
            case data::EquipPosition::Count:
            case data::EquipPosition::None:
                break;
            }
        }
    }

    void GamePacketBuilder::Build(network::game::Equipment& result, const InventoryItem& item, int32_t type)
    {
        result.id = item.itemDataId;
        result.type = type;
        result.attack = item.attack.value_or(0);
        result.defence = item.attack.value_or(0);
        result.str = item.attack.value_or(0);
        result.dex = item.attack.value_or(0);
        result.intell = item.attack.value_or(0);
    }

    void GamePacketBuilder::Build(network::game::Position& position, const GamePlayerSnapshot& playerView)
    {
        position.x = static_cast<float>(playerView.GetPosition().x());
        position.y = static_cast<float>(playerView.GetPosition().y());
        position.z = static_cast<float>(playerView.GetPosition().z());
    }
}
