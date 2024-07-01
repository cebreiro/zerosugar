#include "inventory_change_transaction.h"

namespace zerosugar::xr
{
    auto InventoryChangeTransaction::GetLogs() const -> std::ranges::ref_view<const container_type>
    {
        return _logs;
    }

    void InventoryChangeTransaction::SetCharacterId(int64_t characterId)
    {
        _characterId = characterId;
    }

    InventoryChangeTransaction::InventoryChangeTransaction(int64_t characterId)
        : _characterId(characterId)
    {
    }

    bool InventoryChangeTransaction::Empty() const
    {
        return _logs.empty();
    }

    auto InventoryChangeTransaction::GetCharacterId() const -> int64_t
    {
        return _characterId;
    }

    auto InventoryChangeTransaction::GetLogCount() const -> int64_t
    {
        return std::ssize(_logs);
    }
}
