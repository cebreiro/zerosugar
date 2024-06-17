#pragma once
#include <cstdint>
#include "zerosugar/xr/service/model/generated/coordination_service_message.h"
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/channel/channel.h"

namespace zerosugar::xr { class RPCClient; }

namespace zerosugar::xr::service
{
    class ICoordinationService : public IService
    {
    public:
        static constexpr const char* name = "CoordinationService";

    public:
        virtual ~ICoordinationService() = default;

        virtual auto GetNameAsync(GetNameParam param) -> Future<GetNameResult> = 0;
        virtual auto RequestSnowflakeKeyAsync(RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult> = 0;
        virtual auto ReturnSnowflakeKeyAsync(ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult> = 0;
        virtual auto AddPlayerAsync(AddPlayerParam param) -> Future<AddPlayerResult> = 0;
        auto GetName() const -> std::string_view override { return "CoordinationService"; }
    };

    class CoordinationServiceProxy final
        : public ICoordinationService
        , public std::enable_shared_from_this<CoordinationServiceProxy>
    {
    public:
        explicit CoordinationServiceProxy(SharedPtrNotNull<RPCClient> client);

        auto GetNameAsync(GetNameParam param) -> Future<GetNameResult> override;
        auto RequestSnowflakeKeyAsync(RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult> override;
        auto ReturnSnowflakeKeyAsync(ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult> override;
        auto AddPlayerAsync(AddPlayerParam param) -> Future<AddPlayerResult> override;
    private:
        SharedPtrNotNull<RPCClient> _client;
    };

    void Configure(const SharedPtrNotNull<ICoordinationService>& service, RPCClient& rpcClient);
}
