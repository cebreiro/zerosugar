#pragma once
#include "zerosugar/shared/type/enum_class.h"

namespace zerosugar::sl::gamedata::meta::sox
{
    ENUM_CLASS(ValueType, int32_t,
        (None, 0)
        (String)
        (Int)
        (Float)
    )

    struct Column
    {
        ValueType valueType = ValueType::None;
        std::string name;
    };

    auto ToValueType(int32_t value) -> ValueType;
}
