#pragma once
#include <cstdint>
#include "zerosugar/xr/service/model/generated/orchestrator_service_generated.h"
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/channel/channel.h"

namespace zerosugar::xr::service
{
    class IOrchestratorService : public IService
    {
    public:
        static constexpr std::string name = "OrchestratorService";

    public:
        virtual ~IOrchestratorService() = default;

    };
}
