#include "player_item_change.h"

#include "zerosugar/xr/server/game/instance/contents/inventory/inventory_change_transaction.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/component/inventory_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/player_component.h"

namespace zerosugar::xr::game_task
{
    PlayerItemChange::PlayerItemChange(std::vector<ItemChange> changes, game_time_point_type creationTimePoint)
        : GameTaskParamT(creationTimePoint, MultiTargetSelector(changes | std::views::transform([](const ItemChange& change) -> game_entity_id_type
            {
                return change.playerId;
            })))
    {
        SetParam(std::move(changes));
    }

    void PlayerItemChange::Execute(GameExecutionParallel& parallelContext, MultiTargetSelector::target_type targets)
    {
        (void)parallelContext;

        std::vector<ItemChange>& param = MutableParam();

        std::ranges::sort(param, [](const ItemChange& lhs, const ItemChange& rhs) -> bool
            {
                return lhs.playerId < rhs.playerId;
            });

        IGameRepository& repository = parallelContext.GetServiceLocator().Get<IGameRepository>();

        std::vector<InventoryChangeTransaction> transactions;

        game_entity_id_type currentId;
        InventoryComponent* inventoryComponent = nullptr;
        InventoryChangeTransaction* currentTransaction = nullptr;

        for (const ItemChange& itemChange : param)
        {
            if (!inventoryComponent || currentId != itemChange.playerId)
            {
                auto iter = std::ranges::find_if(targets, [&itemChange](PtrNotNull<const GameEntity> entity) -> bool
                    {
                        return entity->GetId() == itemChange.playerId;
                    });
                if (iter == targets.end())
                {
                    continue;
                }

                PtrNotNull<GameEntity> entity = (*iter);
                currentId = itemChange.playerId;
                inventoryComponent = &entity->GetComponent<InventoryComponent>();

                currentTransaction = &transactions.emplace_back();
                currentTransaction->SetCharacterId(entity->GetComponent<PlayerComponent>().GetCharacterId());
            }

            std::visit([&]<typename T>(const T& param)
            {
                if constexpr (std::is_same_v<T, ItemAdd>)
                {
                    int32_t remain = param.quantity;

                    while (remain > 0)
                    {
                        int32_t resultQuantity = 0;
                        if (inventoryComponent->CanStackItem(param.itemId, remain, resultQuantity))
                        {
                            [[maybe_unused]]
                            const bool added = inventoryComponent->StackItem(param.itemId, resultQuantity);
                            assert(added);

                            remain -= resultQuantity;

                            continue;
                        }

                        if (inventoryComponent->HasEmptySlot())
                        {
                            const int64_t itemUid = repository.PublishItemUniqueId();

                            [[maybe_unused]]
                            const bool added = inventoryComponent->AddItem(itemUid, param.itemId, remain);
                            assert(added);

                            break;
                        }
                    }
                }
                else if constexpr (std::is_same_v<T, ItemRemove>)
                {
                    (void)inventoryComponent->RemoveItem(param.slot);
                }
                else
                {
                    static_assert(!sizeof(T), "not implemented");
                }

            }, itemChange.change);

            currentTransaction->InsertRange(inventoryComponent->GetChangeLogs());
            inventoryComponent->ClearChangeLogs();
        }

        for (InventoryChangeTransaction& transaction : transactions)
        {
            repository.SaveChanges(std::move(transaction));
        }
    }

    void PlayerItemChange::OnComplete(GameExecutionSerial& serialContext)
    {
        // sync
        (void)serialContext;
    }
}
