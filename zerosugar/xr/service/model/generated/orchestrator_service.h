#pragma once
#include <cstdint>
#include "zerosugar/xr/service/model/generated/orchestrator_service_message.h"
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/channel/channel.h"

namespace zerosugar::xr { class RPCClient; }

namespace zerosugar::xr::service
{
    class IOrchestratorService : public IService
    {
    public:
        static constexpr const char* name = "OrchestratorService";

    public:
        virtual ~IOrchestratorService() = default;

    };

    class OrchestratorServiceProxy final
        : public IOrchestratorService
        , public std::enable_shared_from_this<OrchestratorServiceProxy>
    {
    public:
        explicit OrchestratorServiceProxy(SharedPtrNotNull<RPCClient> client);

    private:
        SharedPtrNotNull<RPCClient> _client;
    };
}
