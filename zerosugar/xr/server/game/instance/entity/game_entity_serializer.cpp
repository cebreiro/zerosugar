#include "game_entity_serializer.h"

#include "zerosugar/xr/server/game/instance/component/player_component.h"
#include "zerosugar/xr/server/game/instance/component/inventory_component.h"
#include "zerosugar/xr/server/game/instance/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"

namespace zerosugar::xr
{
    auto GameEntitySerializer::Serialize(const GameEntity& entity) const -> service::DTOCharacter
    {
        (void)entity;

        return {};
    }

    auto GameEntitySerializer::Deserialize(const service::DTOCharacter& character) const -> SharedPtrNotNull<GameEntity>
    {
        auto entity = std::make_shared<GameEntity>();
        entity->AddComponent(std::make_unique<PlayerComponent>(character));
        entity->AddComponent(std::make_unique<MovementComponent>());
        entity->AddComponent(std::make_unique<InventoryComponent>());
        entity->AddComponent(std::make_unique<StatComponent>());

        {
            auto& movement = entity->GetComponent<MovementComponent>();

            Eigen::Vector3d position;
            position.x() = character.x;
            position.y() = character.y;
            position.z() = character.z;

            movement.SetPosition(position);
        }
        {
            auto& inventory = entity->GetComponent<InventoryComponent>();
            auto& statComponent = entity->GetComponent<StatComponent>();

            for (const service::DTOItem& item : character.items)
            {
                InventoryItem inventoryItem;
                inventoryItem.itemId = item.itemId;
                inventoryItem.itemDataId = item.itemDataId;
                inventoryItem.quantity = item.quantity;
                inventoryItem.attack = item.attack;
                inventoryItem.defence = item.defence;
                inventoryItem.str = item.str;
                inventoryItem.dex = item.dex;
                inventoryItem.intell = item.intell;

                [[maybe_unused]]
                const bool added = inventory.AddItem(inventoryItem);
                assert(added);
            }

            for (const service::DTOEquipment& equipment : character.equipments)
            {
                const bool equipped = inventory.Equip(static_cast<data::EquipPosition>(equipment.equipPosition), equipment.itemId);
                if (equipped)
                {
                    if (const InventoryItem* equipItem = inventory.FindItem(equipment.itemId); equipItem)
                    {
                        if (equipItem->attack)
                        {
                            statComponent.AddItemStat(StatType::Attack, StatValue(*equipItem->str));
                        }

                        if (equipItem->defence)
                        {
                            statComponent.AddItemStat(StatType::Attack, StatValue(*equipItem->defence));
                        }

                        if (equipItem->str)
                        {
                            statComponent.AddItemStat(StatType::Str, StatValue(*equipItem->str));
                        }

                        if (equipItem->dex)
                        {
                            statComponent.AddItemStat(StatType::Str, StatValue(*equipItem->dex));
                        }

                        if (equipItem->intell)
                        {
                            statComponent.AddItemStat(StatType::Str, StatValue(*equipItem->intell));
                        }
                    }
                    else
                    {
                        assert(false);
                    }
                }
            }
        }

        return entity;
    }
}
