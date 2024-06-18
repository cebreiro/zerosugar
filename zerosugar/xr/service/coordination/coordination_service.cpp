#include "coordination_service.h"

#include <boost/scope/scope_exit.hpp>
#include "zerosugar/shared/snowflake/snowflake.h"
#include "zerosugar/xr/service/coordination/node/game_server.h"
#include "zerosugar/xr/service/coordination/node/node_container.h"
#include "zerosugar/xr/service/coordination/node/game_instance.h"
#include "zerosugar/xr/service/model/generated/login_service.h"

namespace zerosugar::xr
{
    CoordinationService::CoordinationService(SharedPtrNotNull<execution::IExecutor> executor)
        : _executor(std::move(executor))
        , _strand(std::make_shared<Strand>(_executor))
        , _nodeContainer(std::make_unique<coordination::NodeContainer>())
    {
    }

    CoordinationService::~CoordinationService()
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

    auto CoordinationService::RegisterServerAsync(service::RegisterServerParam param) -> Future<service::RegisterServerResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::RegisterServerResult result;

        if (self->_nodeContainer->HasServerAddress(param.ip, param.port))
        {
            result.errorCode = service::CoordinationServiceErrorCode::RegisterErrorDuplicatedAddress;

            co_return result;
        }

        const auto serverId = coordination::game_server_id_type(++self->_nextServerId);
        auto server = std::make_shared<coordination::GameServer>(serverId, param.name, param.ip, param.port);

        [[maybe_unused]]
        bool added = self->_nodeContainer->Add(std::move(server));
        assert(added);

        co_return result;
    }

    auto CoordinationService::OpenChannelAsync(AsyncEnumerable<service::CoordinationChannelInput> param)
        -> AsyncEnumerable<service::CoordinationChannelOutput>
    {
        auto channel = std::make_shared<Channel<service::CoordinationChannelOutput>>();

        Post(*_strand, [](
            SharedPtrNotNull<CoordinationService> self,
            AsyncEnumerable<service::CoordinationChannelInput> enumerable,
            SharedPtrNotNull<Channel<service::CoordinationChannelOutput>> outputChannel) -> Future<void>
            {
                try
                {
                    while (enumerable.HasNext())
                    {
                        service::CoordinationChannelInput input = co_await enumerable;
                        (void)input;

                        switch (input.opcode)
                        {
                            
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    ZEROSUGAR_LOG_ERROR(self->_serviceLocator,
                        std::format("[{}] coordination channel handler throws. exception: {}", e.what()));
                }

                co_return;

            }, shared_from_this(), std::move(param), channel);

        return AsyncEnumerable<service::CoordinationChannelOutput>(channel);
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

            const auto finder = [zone = param.zoneId](const coordination::GameInstance* instance)
                {
                    return instance->GetZoneId() == zone;
                };
            const auto range = _nodeContainer->GetGameInstanceRange();
            const auto iter = std::ranges::find_if(range, finder);

            if (iter != range.end())
            {
                coordination::GameInstance* gameInstance = *iter;
                coordination::GameServer* server = gameInstance->GetParent();
                assert(server);

                server->Send();
            }
            else
            {
                
            }



            result.ip = "";
            result.port = 0;
        }
        else
        {
            ZEROSUGAR_LOG_ERROR(_serviceLocator,
                std::format("[{}] fail to authenticate. error: {}", GetName(), GetEnumName(authResult.errorCode)));

            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorFailAuthentication;
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
}
