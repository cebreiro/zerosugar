#pragma once
#include <cstdint>
#include "zerosugar/xr/service/model/generated/database_service_message.h"
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
    };

    class DatabaseServiceProxy final
        : public IDatabaseService
        , public std::enable_shared_from_this<DatabaseServiceProxy>
    {
    public:
        explicit DatabaseServiceProxy(SharedPtrNotNull<RPCClient> client);

        auto AddAccountAsync(AddAccountParam param) -> Future<AddAccountResult> override;
        auto GetAccountAsync(GetAccountParam param) -> Future<GetAccountResult> override;
    private:
        SharedPtrNotNull<RPCClient> _client;
    };
}
