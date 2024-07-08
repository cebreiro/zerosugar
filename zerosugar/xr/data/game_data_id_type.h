#pragma once

namespace zerosugar::xr
{
    namespace detail::data
    {
        class MapDataIdTag;
        class MonsterDataIdTag;
    }

    using map_data_id_type = ValueType<int32_t, detail::data::MapDataIdTag>;
    using monster_data_id_type = ValueType<int32_t, detail::data::MonsterDataIdTag>;
}
