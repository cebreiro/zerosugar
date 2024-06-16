#pragma once
#include <cstdint>
#include "zerosugar/xr/service/model/generated/game_service_message.h"
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/channel/channel.h"

namespace zerosugar::xr { class RPCClient; }

namespace zerosugar::xr::service
{
    class IGameService : public IService
    {
    public:
        static constexpr const char* name = "GameService";

    public:
        virtual ~IGameService() = default;

        virtual auto GetNameAsync(GetNameParam param) -> Future<GetNameResult> = 0;
        virtual auto RequestSnowflakeKeyAsync(RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult> = 0;
        virtual auto ReturnSnowflakeKeyAsync(ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult> = 0;
        auto GetName() const -> std::string_view override { return "GameService"; }
    };

    class GameServiceProxy final
        : public IGameService
        , public std::enable_shared_from_this<GameServiceProxy>
    {
    public:
        explicit GameServiceProxy(SharedPtrNotNull<RPCClient> client);

        auto GetNameAsync(GetNameParam param) -> Future<GetNameResult> override;
        auto RequestSnowflakeKeyAsync(RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult> override;
        auto ReturnSnowflakeKeyAsync(ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult> override;
    private:
        SharedPtrNotNull<RPCClient> _client;
    };

    void Configure(const SharedPtrNotNull<IGameService>& service, RPCClient& rpcClient);
}
