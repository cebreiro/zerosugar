#pragma once

namespace zerosugar::xr
{
    namespace detail
    {
        class GameControllerIdTypeTag;
    }

    using game_controller_id_type = ValueType<int64_t, detail::GameControllerIdTypeTag>;
}
