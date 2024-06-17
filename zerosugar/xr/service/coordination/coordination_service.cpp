#include "coordination_service.h"

#include <boost/scope/scope_exit.hpp>
#include "zerosugar/shared/snowflake/snowflake.h"
#include "zerosugar/xr/service/model/generated/login_service.h"

namespace zerosugar::xr
{
    CoordinationService::CoordinationService(SharedPtrNotNull<execution::IExecutor> executor)
        : _executor(std::move(executor))
        , _strand(std::make_shared<Strand>(_executor))
    {
    }

    void CoordinationService::Initialize(ServiceLocator& serviceLocator)
    {
        ICoordinationService::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;
    }

    void CoordinationService::Shutdown()
    {
        ICoordinationService::Shutdown();
    }

    void CoordinationService::Join(std::vector<boost::system::error_code>& errors)
    {
        ICoordinationService::Join(errors);
    }

    auto CoordinationService::RequestSnowflakeKeyAsync(service::RequestSnowflakeKeyParam param) -> Future<service::RequestSnowflakeKeyResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        const std::optional<int32_t>& key = PublishSnowflakeKey(param.requester);

        service::RequestSnowflakeKeyResult result;

        if (key.has_value())
        {
            ZEROSUGAR_LOG_INFO(_serviceLocator,
                std::format("[{}] publish snowflake. id: {}, requester: {}", GetName(), *key, param.requester));

            result.snowflakeKey = *key;
        }
        else
        {
            ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                std::format("[{}] fail to publish snowflake. reuqester: {}", GetName(), param.requester));

            result.errorCode = service::CoordinationServiceErrorCode::RequestSnowflakeKeyErrorOutOfPool;
        }

        co_return result;
    }

    auto CoordinationService::ReturnSnowflakeKeyAsync(service::ReturnSnowflakeKeyParam param) -> Future<service::ReturnSnowflakeKeyResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::ReturnSnowflakeKeyResult result;
        result.errorCode = service::CoordinationServiceErrorCode::ReturnSnowflakeKeyErrorInvalidKey;

        const auto& [begin, end] = _publishedSnowflakeKeys.equal_range(param.requester);
        for (auto iter = begin; iter != end; ++iter)
        {
            if (iter->second == param.snowflakeKey)
            {
                _returnedSnowflakeKeys.emplace(param.snowflakeKey);
                _publishedSnowflakeKeys.erase(iter);

                result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorNone;

                break;
            }
        }

        co_return result;
    }

    auto CoordinationService::AddPlayerAsync(service::AddPlayerParam param) -> Future<service::AddPlayerResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::AddPlayerResult result;

        service::AuthenticateParam authParam;
        authParam.token = param.authenticationToken;

        service::AuthenticateResult authResult = co_await _serviceLocator.Get<service::ILoginService>().AuthenticateAsync(std::move(authParam));
        if (authResult.errorCode == service::LoginServiceErrorCode::LoginErrorNone)
        {
            // select game server
            // register...

            result.ip = "";
            result.port = 0;
        }
        else
        {
            ZEROSUGAR_LOG_ERROR(_serviceLocator,
                std::format("[{}] fail to authenticate. error: {}", GetName(), GetEnumName(authResult.errorCode)));

            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;
        }

        co_return result;
    }

    auto CoordinationService::PublishSnowflakeKey(const std::string& requester) -> std::optional<int32_t>
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

    auto CoordinationService::GetNameAsync(service::GetNameParam param) -> Future<service::GetNameResult>
    {
        (void)param;

        service::GetNameResult result;
        result.name = "default";

        co_return result;
    }
}
