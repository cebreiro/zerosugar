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

        virtual auto RegisterServerAsync(RegisterServerParam param) -> Future<RegisterServerResult> = 0;
        virtual auto UpdateServerStatusAsync(UpdateServerStatusParam param) -> Future<UpdateServerStatusResult> = 0;
        virtual auto OpenChannelAsync(AsyncEnumerable<CoordinationCommandResponse> param) -> AsyncEnumerable<CoordinationCommand> = 0;
        virtual auto RequestSnowflakeKeyAsync(RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult> = 0;
        virtual auto ReturnSnowflakeKeyAsync(ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult> = 0;
        virtual auto AddPlayerAsync(AddPlayerParam param) -> Future<AddPlayerResult> = 0;
        virtual auto RemovePlayerAsync(RemovePlayerParam param) -> Future<RemovePlayerResult> = 0;
        virtual auto AuthenticatePlayerAsync(AuthenticatePlayerParam param) -> Future<AuthenticatePlayerResult> = 0;
        virtual auto BroadcastChattingAsync(BroadcastChattingParam param) -> Future<BroadcastChattingResult> = 0;
        virtual auto RequestDungeonMatchAsync(RequestDungeonMatchParam param) -> Future<RequestDungeonMatchResult> = 0;
        virtual auto CancelDungeonMatchAsync(CancelDungeonMatchParam param) -> Future<CancelDungeonMatchResult> = 0;
        virtual auto ApproveDungeonMatchAsync(ApproveDungeonMatchParam param) -> Future<ApproveDungeonMatchResult> = 0;
        virtual auto RejectDungeonMatchAsync(RejectDungeonMatchParam param) -> Future<RejectDungeonMatchResult> = 0;
        auto GetName() const -> std::string_view override { return "CoordinationService"; }
    };

    class CoordinationServiceProxy final
        : public ICoordinationService
        , public std::enable_shared_from_this<CoordinationServiceProxy>
    {
    public:
        explicit CoordinationServiceProxy(SharedPtrNotNull<RPCClient> client);

        auto RegisterServerAsync(RegisterServerParam param) -> Future<RegisterServerResult> override;
        auto UpdateServerStatusAsync(UpdateServerStatusParam param) -> Future<UpdateServerStatusResult> override;
        auto OpenChannelAsync(AsyncEnumerable<CoordinationCommandResponse> param) -> AsyncEnumerable<CoordinationCommand> override;
        auto RequestSnowflakeKeyAsync(RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult> override;
        auto ReturnSnowflakeKeyAsync(ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult> override;
        auto AddPlayerAsync(AddPlayerParam param) -> Future<AddPlayerResult> override;
        auto RemovePlayerAsync(RemovePlayerParam param) -> Future<RemovePlayerResult> override;
        auto AuthenticatePlayerAsync(AuthenticatePlayerParam param) -> Future<AuthenticatePlayerResult> override;
        auto BroadcastChattingAsync(BroadcastChattingParam param) -> Future<BroadcastChattingResult> override;
        auto RequestDungeonMatchAsync(RequestDungeonMatchParam param) -> Future<RequestDungeonMatchResult> override;
        auto CancelDungeonMatchAsync(CancelDungeonMatchParam param) -> Future<CancelDungeonMatchResult> override;
        auto ApproveDungeonMatchAsync(ApproveDungeonMatchParam param) -> Future<ApproveDungeonMatchResult> override;
        auto RejectDungeonMatchAsync(RejectDungeonMatchParam param) -> Future<RejectDungeonMatchResult> override;
    private:
        SharedPtrNotNull<RPCClient> _client;
    };

    void Configure(const SharedPtrNotNull<ICoordinationService>& service, RPCClient& rpcClient);
}
