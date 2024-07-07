#pragma once
#include "zerosugar/xr/navigation/navi_vector.h"

namespace zerosugar::xr::navi
{
    enum class DrawColor
    {
        Red,
        Blue,
        Green,
        Brown,
        Cyan,
        Yellow,
        LightBlue,
    };

    auto ToInt(DrawColor color) -> uint32_t;

    struct AddVisualizeTargetParam
    {
        int64_t id = 0;
        FVector position = {};
        float radius = 0.f;
        DrawColor color = DrawColor::Green;
    };

    struct RemoveVisualizeTargetParam
    {
        int64_t id = 0;
    };

    struct UpdateVisualizeTargetParam
    {
        int64_t id = 0;
        FVector position = {};

        std::optional<FVector> destPosition = std::nullopt;
        std::optional<DrawColor> destPositionDrawColor = std::nullopt;
    };
}
