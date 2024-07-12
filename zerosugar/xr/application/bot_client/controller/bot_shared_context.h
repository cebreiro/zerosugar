#pragma once
#include "zerosugar/xr/navigation/navigation_service.h"

namespace zerosugar::xr::bot
{
    struct SharedContext
    {
        SharedPtrNotNull<Strand> naviStrand;
        std::unordered_map<int32_t, std::pair<int32_t, SharedPtrNotNull<NavigationService>>> navigationServices;
    };
}
