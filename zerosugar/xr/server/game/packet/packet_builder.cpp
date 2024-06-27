#include "packet_builder.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/component/inventory_component.h"
#include "zerosugar/xr/server/game/instance/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/component/player_component.h"
#include "zerosugar/xr/server/game/instance/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_view_container.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_container.h"

namespace zerosugar::xr
{
    void GamePacketBuilder::Build(network::game::sc::EnterGame& packet,
        const GameInstance& gameInstance, const GameEntity& entity, const GameSpatialSector& sector)
    {
        packet.zoneId = gameInstance.GetZoneId();

        const GameEntityViewContainer& viewContainer = gameInstance.GetEntityViewContainer();
        {
            for (game_entity_id_type id : sector.GetEntities())
            {
                if (id == entity.GetId())
                {
                    continue;
                }

                if (const GameEntityView* entityView = viewContainer.Find(id); entityView)
                {
                    (void)entityView;
                }
                else
                {
                    assert(false);
                }
            }

            packet.remotePlayersCount = 0;
            packet.monstersCount = 0;
        }

        packet.localPlayer = [&]() -> network::game::Player
            {
                network::game::Player player;
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

                return player;
            }();
    }

    void GamePacketBuilder::Build(network::game::PlayerBase& base, const GameEntity& entity)
    {
        auto& playerComponent = entity.GetComponent<PlayerComponent>();
        auto& statComponent = entity.GetComponent<StatComponent>();

        base.hp = 100;// statComponent.GetHP().As<float>();
        base.maxHP = 100;// statComponent.GetMaxHP().As<float>();
        base.attackMin = statComponent.Get(StatType::Attack).As<float>();
        base.attackMax = statComponent.Get(StatType::Attack).As<float>();
        base.speed = 10.f;
        base.name = playerComponent.GetName();
        base.level = playerComponent.GetLevel();
        base.gender = 0;
        base.face = playerComponent.GetFaceId();
        base.hair = playerComponent.GetHairId();
        base.str = statComponent.Get(StatType::Str).As<int32_t>();
        base.dex = statComponent.Get(StatType::Dex).As<int32_t>();
        base.intell = statComponent.Get(StatType::Intell).As<int32_t>();
    }

    void GamePacketBuilder::Build(network::game::PlayerEquipment& equipment, const GameEntity& entity)
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

            const auto make = [](const InventoryItem& item, int32_t type)
                {
                    network::game::Equipment result;
                    result.id = item.itemDataId;
                    result.type = type;
                    result.attack = item.attack.value_or(0);
                    result.defence = item.attack.value_or(0);
                    result.str = item.attack.value_or(0);
                    result.dex = item.attack.value_or(0);
                    result.intell = item.attack.value_or(0);

                    return result;
                };

            switch (static_cast<data::EquipPosition>(i))
            {
            case data::EquipPosition::Armor:
                equipment.armor = make(*item, i);
                break;
            case data::EquipPosition::Gloves:
                equipment.gloves = make(*item, i);;
                break;
            case data::EquipPosition::Shoes:
                equipment.shoes = make(*item, i);;
                break;
            case data::EquipPosition::Weapon:
                equipment.weapon = make(*item, i);;
                break;
            case data::EquipPosition::Count:
            case data::EquipPosition::None:
                break;
            }
        }
    }
}
