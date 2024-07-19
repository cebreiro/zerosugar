#pragma once
#include "zerosugar/xr/service/model/generated/database_item_log_message.h"

namespace zerosugar::xr
{
    using inventory_change_log_type = std::variant<
        service::EquipItemLog,
        service::UnequipItemLog,
        service::ShiftItemLog,
        service::AddItemLog,
        service::RemoveItemLog,
        service::ChangeItemQuantityLog
    >;
}
