#pragma once
#include "zerosugar/xr/navigation/navi_vector.h"

namespace zerosugar::xr::event
{
    struct NaviCompleteRandomPointAroundCircle
    {
        std::optional<navi::FVector> point = std::nullopt;
    };

    struct NaviCompleteFindStraightPath
    {
        boost::container::static_vector<navi::FVector, navi::constant::max_straight_path_count> points;
    };
}
