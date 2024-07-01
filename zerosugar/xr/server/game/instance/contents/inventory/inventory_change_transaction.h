#pragma once
#include <boost/container/small_vector.hpp>
#include "zerosugar/xr/server/game/instance/contents/inventory/inventory_change_log.h"

namespace zerosugar::xr
{
    class InventoryChangeTransaction
    {
    public:
        using container_type = boost::container::small_vector<inventory_change_log_type, 4>;

    public:
        InventoryChangeTransaction(const InventoryChangeTransaction& other) = delete;
        InventoryChangeTransaction& operator=(const InventoryChangeTransaction& other) = delete;

    public:
        InventoryChangeTransaction(InventoryChangeTransaction&& other) noexcept = default;
        InventoryChangeTransaction& operator=(InventoryChangeTransaction&& other) noexcept = default;

        InventoryChangeTransaction() = default;
        explicit  InventoryChangeTransaction(int64_t characterId);

        bool Empty() const;

        template <typename T> requires std::is_same_v<std::ranges::range_value_t<T>, inventory_change_log_type>
        void InsertRange(T&& range);

        auto GetLogCount() const -> int64_t;

        auto GetCharacterId() const -> int64_t;
        auto GetLogs() const -> std::ranges::ref_view<const container_type>;

        void SetCharacterId(int64_t characterId);

    private:
        int64_t _characterId = 0;
        boost::container::small_vector<inventory_change_log_type, 4> _logs;
    };

    template <typename T> requires std::is_same_v<std::ranges::range_value_t<T>, inventory_change_log_type>
    void InventoryChangeTransaction::InsertRange(T&& range)
    {
        std::ranges::copy(std::forward<T>(range), std::back_inserter(_logs));
    }
}
