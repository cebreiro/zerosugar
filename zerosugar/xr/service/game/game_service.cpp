#include "game_service.h"

#include <boost/scope/scope_exit.hpp>

#include "zerosugar/shared/snowflake/snowflake.h"

namespace zerosugar::xr
{
    GameService::GameService(SharedPtrNotNull<execution::IExecutor> executor)
        : _executor(std::move(executor))
        , _strand(std::make_shared<Strand>(_executor))
    {
    }

    void GameService::Initialize(ServiceLocator& serviceLocator)
    {
        IGameService::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;
    }

    void GameService::Shutdown()
    {
        IGameService::Shutdown();
    }

    void GameService::Join(std::vector<boost::system::error_code>& errors)
    {
        IGameService::Join(errors);
    }

    auto GameService::RequestSnowflakeKeyAsync(service::RequestSnowflakeKeyParam param) -> Future<service::RequestSnowflakeKeyResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        const std::optional<int32_t>& key = PublishSnowflakeKey(param.requester);

        service::RequestSnowflakeKeyResult result;
        result.errorCode = key ? service::GameServiceErrorCode::GameErrorNone : service::GameServiceErrorCode::RequestSnowflakeKeyErrorOutOfPool;
        result.snowflakeKey = key.value_or(-1);

        co_return result;
    }

    auto GameService::ReturnSnowflakeKeyAsync(service::ReturnSnowflakeKeyParam param) -> Future<service::ReturnSnowflakeKeyResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::ReturnSnowflakeKeyResult result;
        result.errorCode = service::GameServiceErrorCode::ReturnSnowflakeKeyErrorInvalidKey;

        const auto& [begin, end] = _publishedSnowflakeKeys.equal_range(param.requester);
        for (auto iter = begin; iter != end; ++iter)
        {
            if (iter->second == param.snowflakeKey)
            {
                _returnedSnowflakeKeys.emplace(param.snowflakeKey);
                _publishedSnowflakeKeys.erase(iter);

                result.errorCode = service::GameServiceErrorCode::GameErrorNone;

                break;
            }
        }

        co_return result;
    }

    auto GameService::PublishSnowflakeKey(const std::string& requester) -> std::optional<int32_t>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        std::optional<int32_t> key = std::nullopt;
        boost::scope::scope_exit exit([this, &key, &requester]()
            {
                if (key.has_value())
                {
                    _publishedSnowflakeKeys.insert(std::make_pair(requester, *key));
                }
            });

        constexpr int32_t max = (1 << (snowflake::MACHINE_BIT + snowflake::DATACENTER_BIT));

        if (int32_t next = _nextSnowflake++; next <= max)
        {
            key = next;
        }
        else if (!_returnedSnowflakeKeys.empty())
        {
            key = _returnedSnowflakeKeys.front();
            _returnedSnowflakeKeys.pop();
        }

        return key;
    }

    auto GameService::GetNameAsync(service::GetNameParam param) -> Future<service::GetNameResult>
    {
        (void)param;

        service::GetNameResult result;
        result.name = "default_game_service";

        co_return result;
    }
}
