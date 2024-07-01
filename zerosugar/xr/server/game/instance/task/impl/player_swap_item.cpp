#include "player_swap_item.h"

#include "zerosugar/xr/network/model/generated/game_cs_message_json.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/component/inventory_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/player_component.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/packet/packet_builder.h"

namespace zerosugar::xr::game_task
{
    PlayerSwapItem::PlayerSwapItem(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId, game_time_point_type creationTimePoint)
        : GameTaskBaseParamT(creationTimePoint, std::move(param), MainTargetSelector(targetId))
        , _id(targetId)
    {
    }

    void PlayerSwapItem::Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type target)
    {
        auto& inventoryComponent = target->GetComponent<InventoryComponent>();
        const network::game::cs::SwapItem& param = GetParam();

        do
        {
            if (param.srcEquipped && param.destEquipped)
            {
                break;
            }

            _sync.srcEquipment = param.srcEquipped;
            _sync.destEquipment = param.destEquipped;

            if (param.srcEquipped)
            {
                assert(!param.destEquipped);

                const auto position = static_cast<data::EquipPosition>(param.srcPosition);
                _equipPosition = position;

                if (!inventoryComponent.Unequip(position, param.destPosition))
                {
                    break;
                }

                const auto* item = inventoryComponent.FindItemBySlot(InventoryItemSlotType::Equipment, param.srcPosition);
                _newEquipment = item ? *item : std::optional<InventoryItem>{};
            }
            else
            {
                const InventoryItem* src = inventoryComponent.FindItemBySlot(InventoryItemSlotType::Inventory, param.srcPosition);
                if (!src)
                {
                    break;
                }

                if (param.destEquipped)
                {
                    const auto position = static_cast<data::EquipPosition>(param.destPosition);
                    _equipPosition = position;

                    if (!inventoryComponent.Equip(position, param.srcPosition))
                    {
                        break;
                    }

                    const auto* item = inventoryComponent.FindItemBySlot(InventoryItemSlotType::Equipment, param.destPosition);
                    _newEquipment = item ? *item : std::optional<InventoryItem>{};
                }
                else
                {
                    if (!inventoryComponent.ShiftItem(param.srcPosition, param.destPosition))
                    {
                        break;
                    }
                }
            }

            if (const auto* srcPosItem = inventoryComponent.FindItemBySlot(
                _sync.srcEquipment ? InventoryItemSlotType::Equipment : InventoryItemSlotType::Inventory, param.srcPosition))
            {
                _sync.srcHasItem = true;
                GamePacketBuilder::Build(_sync.srcItem, *srcPosItem);
            }

            if (const auto* destPosItem = inventoryComponent.FindItemBySlot(
                _sync.destEquipment ? InventoryItemSlotType::Equipment : InventoryItemSlotType::Inventory, param.destPosition))
            {
                _sync.destHasItem = true;
                GamePacketBuilder::Build(_sync.destItem, *destPosItem);
            }

            _sync.srcItem.slot = param.srcPosition;
            _sync.destItem.slot = param.destPosition;

            InventoryChangeTransaction transaction;
            transaction.SetCharacterId(inventoryComponent.GetCharacterId());
            transaction.InsertRange(inventoryComponent.GetChangeLogs());

            inventoryComponent.ClearChangeLogs();
            assert(!transaction.Empty());

            parallelContext.GetServiceLocator().Get<IGameRepository>().SaveChanges(std::move(transaction));

            return;
            
        } while (false);

        auto& playerComponent = target->GetComponent<PlayerComponent>();

        nlohmann::json j;
        to_json(j, param);

        ZEROSUGAR_LOG_WARN(parallelContext.GetServiceLocator(),
            std::format("[player_swap_equip_item] invalid request. cid: {}, name: {}, param: {}",
                playerComponent.GetCharacterId(), playerComponent.GetName(), j.dump()));
    }

    void PlayerSwapItem::OnComplete(GameExecutionSerial& serialContext)
    {
        (void)serialContext;

        GamePlayerSnapshot* snapshot = serialContext.GetSnapshotContainer().FindPlayer(_id);
        assert(snapshot);

        serialContext.GetSnapshotView().Send(_sync, snapshot->GetController());

        if (_equipPosition)
        {
            serialContext.GetSnapshotController().ProcessPlayerEquipItemChange(_id, *_equipPosition,
                _newEquipment ? &_newEquipment.value() : nullptr);
        }
    }
}
