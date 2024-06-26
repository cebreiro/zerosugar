#pragma once
#include <cstdint>
#include "zerosugar/xr/service/model/generated/database_service_message.h"
#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/channel/channel.h"

namespace zerosugar::xr { class RPCClient; }

namespace zerosugar::xr::service
{
    class IDatabaseService : public IService
    {
    public:
        static constexpr const char* name = "DatabaseService";

    public:
        virtual ~IDatabaseService() = default;

        virtual auto AddAccountAsync(AddAccountParam param) -> Future<AddAccountResult> = 0;
        virtual auto GetAccountAsync(GetAccountParam param) -> Future<GetAccountResult> = 0;
        virtual auto AddCharacterAsync(AddCharacterParam param) -> Future<AddCharacterResult> = 0;
        virtual auto GetCharacterAsync(GetCharacterParam param) -> Future<GetCharacterResult> = 0;
        virtual auto RemoveCharacterAsync(RemoveCharacterParam param) -> Future<RemoveCharacterResult> = 0;
        virtual auto GetLobbyCharactersAsync(GetLobbyCharactersParam param) -> Future<GetLobbyCharactersResult> = 0;
        auto GetName() const -> std::string_view override { return "DatabaseService"; }
    };

    class DatabaseServiceProxy final
        : public IDatabaseService
        , public std::enable_shared_from_this<DatabaseServiceProxy>
    {
    public:
        explicit DatabaseServiceProxy(SharedPtrNotNull<RPCClient> client);

        auto AddAccountAsync(AddAccountParam param) -> Future<AddAccountResult> override;
        auto GetAccountAsync(GetAccountParam param) -> Future<GetAccountResult> override;
        auto AddCharacterAsync(AddCharacterParam param) -> Future<AddCharacterResult> override;
        auto GetCharacterAsync(GetCharacterParam param) -> Future<GetCharacterResult> override;
        auto RemoveCharacterAsync(RemoveCharacterParam param) -> Future<RemoveCharacterResult> override;
        auto GetLobbyCharactersAsync(GetLobbyCharactersParam param) -> Future<GetLobbyCharactersResult> override;
    private:
        SharedPtrNotNull<RPCClient> _client;
    };

    void Configure(const SharedPtrNotNull<IDatabaseService>& service, RPCClient& rpcClient);
}
