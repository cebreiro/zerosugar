#pragma once
#include "zerosugar/xr/navigation/navi_vector.h"

namespace zerosugar::xr::event
{
    struct NaviCompleteRandomPointAroundCircle
    {
        std::optional<navi::FVector> point = std::nullopt;
    };
}
