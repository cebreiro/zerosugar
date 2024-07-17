#include "navi_visualize_param.h"

#include <recastnavigation/DebugDraw.h>

namespace zerosugar::xr::navi::vis
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
        case DrawColor::White:
            return duRGBA(255, 255, 255, 255);
        }

        return duRGBA(255, 0, 0, 255);
    }

    auto ToArray(DrawColor color) -> std::array<uint8_t, 4>
    {
        switch (color)
        {
        case DrawColor::Red:
            return { 255, 0, 0, 255 };
        case DrawColor::Blue:
            return { 30, 144, 255, 255 };
        case DrawColor::Green:
            return { 50, 205, 50, 255 };
        case DrawColor::Brown:
            return { 50, 20, 12, 255 };
        case DrawColor::Cyan:
            return { 0, 255, 255, 255 };
        case DrawColor::Yellow:
            return { 255, 255, 0, 255 };
        case DrawColor::LightBlue:
            return { 0, 192, 255, 255 };
        case DrawColor::White:
            return { 255, 255, 255, 255 };
        }

        return { 255, 0, 0, 255 };
    }
}
