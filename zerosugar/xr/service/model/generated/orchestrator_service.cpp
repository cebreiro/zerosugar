
#include "orchestrator_service.h"

#include "orchestrator_service_message_json.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"

namespace zerosugar::xr::service
{
    OrchestratorServiceProxy::OrchestratorServiceProxy(SharedPtrNotNull<RPCClient> client)
        : _client(std::move(client))
    {
    }
}
