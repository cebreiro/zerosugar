#pragma once
#include <string>

namespace zerosugar::bt
{
    template <typename T>
    concept bt_event_concept = requires
    {
        requires std::is_copy_assignable_v<T>;
    };

    template <bt_event_concept... E>
    struct Event {};
}
