#include "coordination_service.h"

#include <boost/scope/scope_exit.hpp>
#include "zerosugar/shared/snowflake/snowflake.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/xr/service/coordination/command/command_response_channel_runner.h"
#include "zerosugar/xr/service/coordination/command/command_response_handler_factory.h"
#include "zerosugar/xr/service/coordination/contents/match/dungeon_match_coordinator.h"
#include "zerosugar/xr/service/coordination/load_balance/load_balancer.h"
#include "zerosugar/xr/service/coordination/node/game_instance.h"
#include "zerosugar/xr/service/coordination/node/game_server.h"
#include "zerosugar/xr/service/coordination/node/game_user.h"
#include "zerosugar/xr/service/coordination/node/node_container.h"
#include "zerosugar/xr/service/model/generated/coordination_command_message.h"
#include "zerosugar/xr/service/model/generated/login_service.h"

namespace zerosugar::xr
{
    CoordinationService::CoordinationService(SharedPtrNotNull<execution::IExecutor> executor)
        : _executor(std::move(executor))
        , _strand(std::make_shared<Strand>(_executor))
        , _nodeContainer(std::make_unique<coordination::NodeContainer>())
        , _loadBalancer(std::make_unique<coordination::LoadBalancer>(*this))
        , _commandResponseHandlerFactory(std::make_unique<coordination::CommandResponseHandlerFactory>())
        , _dungeonMatchCoordinator(std::make_shared<DungeonMatchCoordinator>(*this))
    {
    }

    CoordinationService::~CoordinationService()
    {
    }

    void CoordinationService::Initialize(ServiceLocator& serviceLocator)
    {
        ICoordinationService::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;

        Configure(shared_from_this(), serviceLocator.Get<RPCClient>());

        _dungeonMatchCoordinator->Start();
    }

    void CoordinationService::Shutdown()
    {
        ICoordinationService::Shutdown();

        _dungeonMatchCoordinator->Shutdown();
    }

    void CoordinationService::Join(std::vector<boost::system::error_code>& errors)
    {
        ICoordinationService::Join(errors);

        _dungeonMatchCoordinator->Join().Get();
    }

    auto CoordinationService::RegisterServerAsync(service::RegisterServerParam param) -> Future<service::RegisterServerResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::RegisterServerResult result;

        if (self->_nodeContainer->HasServerAddress(param.ip, static_cast<uint16_t>(param.port)))
        {
            result.errorCode = service::CoordinationServiceErrorCode::RegisterErrorDuplicatedAddress;

            co_return result;
        }

        const auto serverId = coordination::game_server_id_type(++self->_nextServerId);
        result.serverId = serverId.Unwrap();

        auto server = std::make_shared<coordination::GameServer>(serverId, param.name, param.ip, static_cast<uint16_t>(param.port));

        [[maybe_unused]]
        bool added = self->_nodeContainer->Add(server);
        assert(added);

        added = _loadBalancer->Add(serverId);
        assert(added);

        co_return result;
    }

    auto CoordinationService::UpdateServerStatusAsync(service::UpdateServerStatusParam param) -> Future<service::UpdateServerStatusResult>
    {
        const auto id = coordination::game_server_id_type(param.serverId);

        [[maybe_unused]]
        const bool updated = _loadBalancer->Update(id, coordination::ServerStatus{
            .loadCPUPercentage = param.loadCPUPercentage,
            .freePhysicalMemoryGB = param.freePhysicalMemoryGB,
            });
        assert(updated);

        service::UpdateServerStatusResult result;

        co_return result;
    }

    auto CoordinationService::OpenChannelAsync(AsyncEnumerable<service::CoordinationCommandResponse> param)
        -> AsyncEnumerable<service::CoordinationCommand>
    {
        auto commandChannel = std::make_shared<Channel<service::CoordinationCommand>>();

        auto runner = std::make_shared<coordination::CommandResponseChannelRunner>(*this, std::move(param), commandChannel);
        runner->Start();

        return AsyncEnumerable<service::CoordinationCommand>(commandChannel);
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
                fmt::format("[{}] publish snowflake. id: {}, requester: {}", GetName(), *key, param.requester));

            result.snowflakeKey = *key;
        }
        else
        {
            ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] fail to publish snowflake. reuqester: {}", GetName(), param.requester));

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
        if (authResult.errorCode != service::LoginServiceErrorCode::LoginErrorNone)
        {
            ZEROSUGAR_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to authenticate. error: {}", GetName(), GetEnumName(authResult.errorCode)));

            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorFailAuthentication;

            co_return result;
        }

        coordination::GameServer* server = _loadBalancer->Select();
        assert(server);

        coordination::GameInstance* instance = server->FindGameInstance(param.zoneId);
        if (!instance)
        {
            const coordination::game_instance_id_type instanceId = PublishGameInstanceId();

            coordination::command::LaunchGameInstance launchGameInstance;
            launchGameInstance.zoneId = param.zoneId;
            launchGameInstance.gameInstanceId = instanceId.Unwrap();

            Future<void> response;
            server->SendCommand(launchGameInstance, response);

            try
            {
                co_await response;
            }
            catch (const std::exception& e)
            {
                ZEROSUGAR_LOG_ERROR(_serviceLocator,
                    fmt::format("[{}] fail to launch game instance. server: [{}, {}:{}], exception: {}",
                        GetName(), server->GetName(), server->GetIP(), server->GetPort(), e.what()));

                result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;

                co_return result;
            }

            instance = server->FindChild(instanceId);
            if (!instance)
            {
                assert(false);

                ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] launch game instance success. but not found instance. server: [{}, {}:{}], instanceId: {}",
                        GetName(), server->GetName(), server->GetIP(), server->GetPort(), instanceId));

                result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;

                co_return result;
            }
        }

        result.ip = server->GetIP();
        result.port = server->GetPort();

        const auto gameUserId = coordination::game_user_id_type(++_nextGameUserId);
        auto gameUser = std::make_shared<coordination::GameUser>(gameUserId, param.authenticationToken, param.accountId, param.characterId, param.zoneId);
        gameUser->SetMigrating(true);

        [[maybe_unused]]
        bool added = _nodeContainer->Add(gameUser);
        assert(added);

        gameUser->SetParent(instance);

        // TODO: register to login service

        co_return result;
    }

    auto CoordinationService::AuthenticatePlayerAsync(service::AuthenticatePlayerParam param)
        -> Future<service::AuthenticatePlayerResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::AuthenticatePlayerResult result;

        do
        {
            coordination::GameUser* user = _nodeContainer->FindGameUser(param.authenticationToken);
            if (!user)
            {
                result.errorCode = service::CoordinationServiceErrorCode::AuthenticatePlayerErrorUserNotFound;

                break;
            }

            if (!user->IsMigrating())
            {
                result.errorCode = service::CoordinationServiceErrorCode::AuthenticatePlayerErrorUserIsNotMigrating;

                break;
            }

            coordination::GameInstance* gameInstance = user->GetParent();
            assert(gameInstance);

            coordination::GameServer* gameServer = gameInstance->GetParent();
            assert(gameServer);

            if (gameServer->GetId() != coordination::game_server_id_type(param.serverId))
            {
                result.errorCode = service::CoordinationServiceErrorCode::AuthenticatePlayerErrorRequestToInvalidServer;

                break;
            }

            user->SetMigrating(false);

            result.accountId = user->GetAccountId();
            result.characterId = user->GetCharacterId();
            result.gameInstanceId = gameInstance->GetId().Unwrap();
            result.userUniqueId = user->GetId().Unwrap();
            
        } while (false);

        co_return result;
    }

    auto CoordinationService::RemovePlayerAsync(service::RemovePlayerParam param) -> Future<service::RemovePlayerResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::RemovePlayerResult result;

        do
        {
            const coordination::GameUser* user = _nodeContainer->FindGameUser(param.authenticationToken);
            if (!user)
            {
                result.errorCode = service::CoordinationServiceErrorCode::AuthenticatePlayerErrorUserNotFound;

                break;
            }

            coordination::GameInstance* gameInstance = user->GetParent();
            coordination::GameServer* server = gameInstance ? gameInstance->GetParent() : nullptr;

            if (!server || server->GetId() != coordination::game_server_id_type(param.serverId))
            {
                result.errorCode = service::CoordinationServiceErrorCode::RemovePlayerErrorInvalidServer;

                break;
            }

            gameInstance->RemoveChild(user->GetId());
            _nodeContainer->Remove(user->GetId());

        } while (false);

        _nodeContainer->Find(coordination::game_server_id_type(param.serverId));

        co_return result;
    }

    auto CoordinationService::RemoveGameInstanceAsync(service::RemoveGameInstanceParam param) -> Future<service::RemoveGameInstanceResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::RemoveGameInstanceResult result;

        do
        {
            const auto instanceId = coordination::game_instance_id_type(param.gameInstanceId);

            coordination::GameInstance* instance = _nodeContainer->Find(instanceId);
            if (!instance)
            {
                result.errorCode = service::CoordinationServiceErrorCode::RemoveGameInstanceErrorInstanceNotFound;

                break;
            }

            if (instance->GetChildCount() > 0)
            {
                result.errorCode = service::CoordinationServiceErrorCode::RemoveGameInstanceErrorInstanceNotEmpty;

                break;
            }

            coordination::GameServer* server = instance->GetParent();
            server->RemoveChild(instanceId);

            [[maybe_unused]]
            const bool removed = _nodeContainer->Remove(instanceId);
            assert(removed);

        } while (false);

        co_return result;
    }

    auto CoordinationService::BroadcastChattingAsync(service::BroadcastChattingParam param) -> Future<service::BroadcastChattingResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::BroadcastChattingResult result;

        const coordination::GameUser* user = FindGameUser(param.authenticationToken,
            coordination::game_instance_id_type(param.gameInstanceId), coordination::game_server_id_type(param.serverId));
        if (!user)
        {
            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;
            co_return result;
        }

        coordination::command::BroadcastChatting command;
        command.message = std::move(param.message);

        for (PtrNotNull<coordination::GameServer> server : _nodeContainer->GetServerRange())
        {
            server->SendCommand(command);
        }

        co_return result;
    }

    auto CoordinationService::RequestDungeonMatchAsync(service::RequestDungeonMatchParam param) -> Future<service::RequestDungeonMatchResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::RequestDungeonMatchResult result;

        const coordination::GameUser* user = FindGameUser(param.authenticationToken,
            coordination::game_instance_id_type(param.gameInstanceId), coordination::game_server_id_type(param.serverId));
        if (!user)
        {
            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;
            co_return result;
        }

        if (!_dungeonMatchCoordinator->AddUser(user->GetId(), param.dungeonId))
        {
            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;
            co_return result;
        }

        co_return result;
    }

    auto CoordinationService::CancelDungeonMatchAsync(service::CancelDungeonMatchParam param) -> Future<service::CancelDungeonMatchResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::CancelDungeonMatchResult result;

        const coordination::GameUser* user = FindGameUser(param.authenticationToken,
            coordination::game_instance_id_type(param.gameInstanceId), coordination::game_server_id_type(param.serverId));
        if (!user)
        {
            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;
            co_return result;
        }

        if (!_dungeonMatchCoordinator->CancelUserMatch(user->GetId()))
        {
            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;
            co_return result;
        }

        co_return result;
    }

    auto CoordinationService::ApproveDungeonMatchAsync(service::ApproveDungeonMatchParam param) -> Future<service::ApproveDungeonMatchResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::ApproveDungeonMatchResult result;

        const coordination::GameUser* user = FindGameUser(param.authenticationToken,
            coordination::game_instance_id_type(param.gameInstanceId), coordination::game_server_id_type(param.serverId));
        if (!user)
        {
            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;
            co_return result;
        }

        if (!_dungeonMatchCoordinator->ApproveUserMatch(user->GetId()))
        {
            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;
            co_return result;
        }

        co_return result;
    }

    auto CoordinationService::RejectDungeonMatchAsync(service::RejectDungeonMatchParam param) -> Future<service::RejectDungeonMatchResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        service::RejectDungeonMatchResult result;

        const coordination::GameUser* user = FindGameUser(param.authenticationToken,
            coordination::game_instance_id_type(param.gameInstanceId), coordination::game_server_id_type(param.serverId));
        if (!user)
        {
            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;
            co_return result;
        }

        if (!_dungeonMatchCoordinator->RejectUserMatch(user->GetId()))
        {
            result.errorCode = service::CoordinationServiceErrorCode::CoordinationErrorInternalError;
            co_return result;
        }

        co_return result;
    }

    auto CoordinationService::PublishGameInstanceId() -> coordination::game_instance_id_type
    {
        return coordination::game_instance_id_type(++_nextGameInstanceId);
    }

    auto CoordinationService::GetStrand() -> Strand&
    {
        return *_strand;
    }

    auto CoordinationService::GetServiceLocator() -> ServiceLocator&
    {
        return _serviceLocator;
    }

    auto CoordinationService::GetNodeContainer() -> coordination::NodeContainer&
    {
        return *_nodeContainer;
    }

    auto CoordinationService::GetLoadBalancer() -> coordination::ILoadBalancer&
    {
        return *_loadBalancer;
    }

    auto CoordinationService::GetChannelInputHandlerFactory() -> const coordination::CommandResponseHandlerFactory&
    {
        return *_commandResponseHandlerFactory;
    }

    auto CoordinationService::FindGameUser(const std::string& token, coordination::game_instance_id_type instanceId,
        coordination::game_server_id_type serverId) -> coordination::GameUser*
    {
        coordination::GameUser* user = _nodeContainer->FindGameUser(token);
        if (!user)
        {
            return nullptr;
        }

        const coordination::GameInstance* instance = user->GetParent();
        if (!instance || instance->GetId() != instanceId)
        {
            return nullptr;
        }

        if (const coordination::GameServer* server = instance->GetParent(); !server || server->GetId() != serverId)
        {
            return nullptr;
        }

        return user;
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
