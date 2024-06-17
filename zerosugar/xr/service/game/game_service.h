#pragma once
#include "zerosugar/xr/service/model/generated/game_service.h"

namespace zerosugar::xr
{
    class GameService final
        : public service::IGameService
        , public std::enable_shared_from_this<GameService>
    {
    public:
        explicit GameService(SharedPtrNotNull<execution::IExecutor> executor);

        void Initialize(ServiceLocator& serviceLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;

        auto GetNameAsync(service::GetNameParam param) -> Future<service::GetNameResult> override;
        auto RequestSnowflakeKeyAsync(service::RequestSnowflakeKeyParam param) -> Future<service::RequestSnowflakeKeyResult> override;
        auto ReturnSnowflakeKeyAsync(service::ReturnSnowflakeKeyParam param) -> Future<service::ReturnSnowflakeKeyResult> override;

    private:
        auto PublishSnowflakeKey(const std::string& requester) -> std::optional<int32_t>;

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<Strand> _strand;

        ServiceLocatorT<ILogService> _serviceLocator;

        std::optional<std::pair<std::string, int32_t>> _lobby;

        int32_t _nextSnowflake = 0;
        std::unordered_multimap<std::string, int32_t> _publishedSnowflakeKeys;
        std::queue<int32_t> _returnedSnowflakeKeys;
    };
}
