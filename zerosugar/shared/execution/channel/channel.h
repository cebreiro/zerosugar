#pragma once
#include <concepts>
#include "zerosugar/shared/execution/channel/impl/mpmc_channel.h"

namespace zerosugar::execution
{
    template <std::move_constructible T>
    using Channel = channel::MpmcChannel<T>;
}
