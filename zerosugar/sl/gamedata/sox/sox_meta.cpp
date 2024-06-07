#include "sox_meta.h"

namespace zerosugar::sl::gamedata::meta::sox
{
    auto ToValueType(int32_t value) -> ValueType
    {
        switch (value)
        {
        case 1:
            return ValueType::String;
        case 2:
        case 3:
        case 5: // PartID
            return ValueType::Int;
        case 4:
            return ValueType::Float;
        default:
            assert(false);
            return ValueType::None;
        }
    }
}
