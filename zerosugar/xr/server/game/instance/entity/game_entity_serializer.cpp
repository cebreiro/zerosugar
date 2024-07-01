#include "game_entity_serializer.h"

#include "zerosugar/xr/server/game/instance/entity/component/player_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/inventory_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"
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

            [[maybe_unused]]
            const bool success = inventory.Initialize(character.characterId, character.items, character.equipments);
            assert(success);
        }

        return entity;
    }
}
