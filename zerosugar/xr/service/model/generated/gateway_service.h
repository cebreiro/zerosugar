#pragma once
#include <cstdint>
#include "zerosugar/xr/service/model/generated/gateway_service_message.h"
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/channel/channel.h"

namespace zerosugar::xr { class RPCClient; }

namespace zerosugar::xr::service
{
    class IGatewayService : public IService
    {
    public:
        static constexpr const char* name = "GatewayService";

    public:
        virtual ~IGatewayService() = default;

        virtual auto AddGameServiceAsync(AddGameServiceParam param) -> Future<AddGameServiceResult> = 0;
        virtual auto GetGameServiceListAsync(GetGameServiceListParam param) -> Future<GetGameServiceListResult> = 0;
        auto GetName() const -> std::string_view override { return "GatewayService"; }
    };

    class GatewayServiceProxy final
        : public IGatewayService
        , public std::enable_shared_from_this<GatewayServiceProxy>
    {
    public:
        explicit GatewayServiceProxy(SharedPtrNotNull<RPCClient> client);

        auto AddGameServiceAsync(AddGameServiceParam param) -> Future<AddGameServiceResult> override;
        auto GetGameServiceListAsync(GetGameServiceListParam param) -> Future<GetGameServiceListResult> override;
    private:
        SharedPtrNotNull<RPCClient> _client;
    };

    void Configure(const SharedPtrNotNull<IGatewayService>& service, RPCClient& rpcClient);
}
