#pragma once
#include <string>

namespace zerosugar::bt
{
    template <typename T>
    concept bt_event_concept = requires
    {
        { T::name } -> std::convertible_to<std::string>;
    };

    template <bt_event_concept... E>
    struct Event {};
}
