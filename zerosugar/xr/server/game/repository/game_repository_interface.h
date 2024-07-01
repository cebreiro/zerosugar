#pragma once
#include "zerosugar/xr/server/game/instance/contents/inventory/inventory_change_transaction.h"
#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"

namespace zerosugar::xr
{
    class IGameRepository : public IService
    {
    public:
        virtual ~IGameRepository() = default;

        virtual auto Find(int64_t characterId) -> Future<std::optional<service::DTOCharacter>> = 0;

        virtual auto FinalizeSaves(int64_t characterId) -> Future<void> = 0;
        virtual void SaveChanges(InventoryChangeTransaction change) = 0;

        virtual auto GetName() const -> std::string_view = 0;
    };
}
