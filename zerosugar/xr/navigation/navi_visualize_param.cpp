#include "navi_visualize_param.h"

#include <recastnavigation/DebugDraw.h>

namespace zerosugar::xr::navi
{
    auto ToInt(DrawColor color) -> uint32_t
    {
        switch (color)
        {
        case DrawColor::Red:
            return duRGBA(255, 0, 0, 255);
        case DrawColor::Blue:
            return duRGBA(30, 144, 255, 255);
        case DrawColor::Green:
            return duRGBA(50, 205, 50, 255);
        case DrawColor::Brown:
            return duRGBA(50, 20, 12, 255);
        case DrawColor::Cyan:
            return duRGBA(0, 255, 255, 255);
        case DrawColor::Yellow:
            return duRGBA(255, 255, 0, 255);
        case DrawColor::LightBlue:
            return duRGBA(0, 192, 255, 255);
        }

        return duRGBA(255, 0, 0, 255);
    }
}
