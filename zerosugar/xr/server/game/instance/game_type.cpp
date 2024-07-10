#include "game_type.h"

namespace zerosugar::xr
{
    auto GetMilliFromGameSeconds(double seconds) -> std::chrono::milliseconds
    {
        return std::chrono::duration_cast<
            std::chrono::milliseconds>(std::chrono::duration<double, std::chrono::seconds::period>(seconds));
    }
}
