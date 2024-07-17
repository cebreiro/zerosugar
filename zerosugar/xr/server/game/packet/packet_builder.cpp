#include "packet_builder.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/entity/component/inventory_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/player_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"

namespace zerosugar::xr
{
    void GamePacketBuilder::Build(network::game::sc::EnterGame& result, const GameEntity& entity, int32_t mapId)
    {
        result.zoneId = mapId;
        Build(result.localPlayer, entity);
    }

    void GamePacketBuilder::Build(network::game::sc::AddRemotePlayer& result, const GamePlayerSnapshot& player)
    {
        Build(result.players.emplace_back(), player);
        result.playersCount = 1;
    }

    void GamePacketBuilder::Build(network::game::sc::RemoveRemotePlayer& result, const GamePlayerSnapshot& player)
    {
        result.players.emplace_back(player.GetId().Unwrap());
        result.playersCount = 1;
    }

    void GamePacketBuilder::Build(network::game::sc::MoveRemotePlayer& result, const GamePlayerSnapshot& player)
    {
        result.id = player.GetId().Unwrap();
        Build(result.position, player);

        result.rotation.yaw = player.GetYaw();
    }

    void GamePacketBuilder::Build(network::game::PlayerInventoryItem& result, const InventoryItem& item)
    {
        result.slot = item.slot;
        result.id = item.itemDataId;
        result.count = item.quantity;
        result.attack = item.attack.value_or(0);
        result.defence = item.attack.value_or(0);
        result.str = item.attack.value_or(0);
        result.dex = item.attack.value_or(0);
        result.intell = item.attack.value_or(0);
    }

    void GamePacketBuilder::Build(network::game::Player& result, const GameEntity& entity)
    {
        result.id = entity.GetId().Unwrap();

        Build(result.base, entity);
        Build(result.equipment, entity);

        {
            auto& movementComponent = entity.GetComponent<MovementComponent>();

            network::game::Position& position = result.transform.position;

            position.x = static_cast<float>(movementComponent.GetX());
            position.y = static_cast<float>(movementComponent.GetY());
            position.z = static_cast<float>(movementComponent.GetZ());

            result.transform.rotation.yaw = static_cast<float>(movementComponent.GetYaw());
        }
        {
            auto& inventoryComponent = entity.GetComponent<InventoryComponent>();

            result.gold = 123123;

            for (const InventoryItem& item : inventoryComponent.GetInventoryItemsRange())
            {
                Build(result.items.emplace_back(), item);

                ++result.itemsCount;
            }
        }
    }

    void GamePacketBuilder::Build(network::game::RemotePlayer& result, const GamePlayerSnapshot& player)
    {
        result.id = player.GetId().Unwrap();

        Build(result.base, player);
        Build(result.equipment, player);
        Build(result.transform.position, player);

        result.transform.rotation.yaw = player.GetYaw();
    }

    void GamePacketBuilder::Build(network::game::PlayerBase& result, const GameEntity& entity)
    {
        auto& playerComponent = entity.GetComponent<PlayerComponent>();
        auto& statComponent = entity.GetComponent<StatComponent>();

        auto now = game_clock_type::now();

        result.hp = statComponent.GetHP(now).As<float>();
        result.maxHp = statComponent.GetMaxHP().As<float>();
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
        result.stamina = statComponent.GetStamina(now).As<float>();
        result.staminaMax = statComponent.GetMaxStamina().As<float>();
    }

    void GamePacketBuilder::Build(network::game::PlayerBase& result, const GamePlayerSnapshot& player)
    {
        result.hp = player.GetHp();
        result.maxHp = player.GetMaxHp();
        result.attackMin = player.GetAttackMin();
        result.attackMax = player.GetAttackMax();
        result.speed = player.GetSpeed();
        result.name = player.GetName();
        result.level = player.GetLevel();
        result.gender = player.GetGender();
        result.face = player.GetFace();
        result.hair = player.GetHair();
        result.str = player.GetStr();
        result.dex = player.GetDex();
        result.intell = player.GetIntell();
        result.stamina = player.GetStamina();
        result.staminaMax = player.GetStaminaMax();
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
                break;
            }
        }
    }

    void GamePacketBuilder::Build(network::game::PlayerEquipment& result, const GamePlayerSnapshot& player)
    {
        const auto& equipItems = player.GetEquipments();

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

    void GamePacketBuilder::Build(network::game::Position& position, const GamePlayerSnapshot& player)
    {
        position.x = static_cast<float>(player.GetPosition().x());
        position.y = static_cast<float>(player.GetPosition().y());
        position.z = static_cast<float>(player.GetPosition().z());
    }

    void GamePacketBuilder::Build(network::game::Monster& result, const GameMonsterSnapshot& snapshot)
    {
        result.dataId = snapshot.GetDataId();
        result.id = snapshot.GetId().Unwrap();
        result.transform.position.x = static_cast<float>(snapshot.GetPosition().x());
        result.transform.position.y = static_cast<float>(snapshot.GetPosition().y());
        result.transform.position.z = static_cast<float>(snapshot.GetPosition().z());
        result.transform.rotation.yaw = snapshot.GetYaw();
        result.hp = snapshot.GetHp();
        result.maxHp = snapshot.GetAttackMax();
        result.attackMin = snapshot.GetAttackMin();
        result.attackMax = snapshot.GetAttackMax();
        result.attackRange = snapshot.GetAttackRange();
        result.attackSpeed = snapshot.GetAttackSpeed();
        result.speed = 50.f;
        result.defence = 0.f;
    }
}
