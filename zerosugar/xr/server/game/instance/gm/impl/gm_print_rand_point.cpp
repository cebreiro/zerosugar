#include "gm_print_rand_point.h"

#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/navigation/navi_vector.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::gm
{
    bool PrintRandomPoint::HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player, const std::optional<int32_t>& count)
    {
        (void)player;

        NavigationService* navigationService = serialContext.GetNavigationService();
        if (!navigationService)
        {
            return false;
        }

        const int64_t randCount = count.value_or(1);

        navigationService->GetRandomPoints(randCount)
            .Then(serialContext.GetStrand(),
                [serial = serialContext.Hold()](const std::vector<navi::FVector>& points)
                {
                    std::ostringstream oss;

                    for (int64_t i = 0; i < std::ssize(points); ++i)
                    {
                        const navi::FVector& point = points[i];

                        oss << "{\n";
                        oss << fmt::format("    \"id\" : {},", 20001);
                        oss << fmt::format("    \"x\" : {:.2f},", point.GetX());
                        oss << fmt::format("    \"y\" : {:.2f},", point.GetY());
                        oss << fmt::format("    \"z\" : {:.2f}", point.GetZ());
                        oss << "}";

                        if (i != std::ssize(points) - 1)
                        {
                            oss << ",";
                        }

                        oss << "\n";
                    }

                    ZEROSUGAR_LOG_INFO(serial->GetServiceLocator(),
                        fmt::format("[gm_command_{}] result: {}",
                            PrintRandomPoint::name, oss.str()));
                });

        return true;
    }
}
