#pragma once

namespace zerosugar::xr
{
    namespace detail
    {
        class GameItemIdTypeTag;
    }

    using game_item_id_type = ValueType<int64_t, detail::GameItemIdTypeTag>;
}
