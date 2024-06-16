#include "lobby_session_state.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/data/enum/equip_position.h"
#include "zerosugar/xr/network/packet_builder.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/model/generated/lobby_cs_message.h"
#include "zerosugar/xr/network/model/generated/lobby_sc_message.h"

namespace zerosugar::xr
{
    LobbyServerSessionStateMachine::LobbyServerSessionStateMachine(ServiceLocator& serviceLocator, Session& session)
        : _session(session.weak_from_this())
        , _name(std::format("lobby_session_state_machine[{}]", session.GetId()))
        , _serviceLocator(serviceLocator)
        , _channel(std::make_shared<Channel<Buffer>>())
    {
        AddState<lobby::ConnectedState>(true, *this, serviceLocator, session)
            .Add(LobbySessionState::Authenticated);

        AddState<lobby::AuthenticatedState>( false, *this, serviceLocator, session)
            .Add(LobbySessionState::TransitionToGame);

        AddState<lobby::TransitionToGameState>(false);
    }

    void LobbyServerSessionStateMachine::Start()
    {
        const std::shared_ptr<Session>& session = _session.lock();
        assert(session);

        Post(session->GetStrand(),
            [](SharedPtrNotNull<LobbyServerSessionStateMachine> self, session::id_type id, WeakPtrNotNull<Session> weak) -> Future<void>
            {
                try
                {
                    co_await self->Run();
                }
                catch (const std::exception& e)
                {
                    ZEROSUGAR_LOG_DEBUG(self->_serviceLocator,
                        std::format("[{}] exit with exception. session_id: {}, exception: {}", self->GetName(), id, e.what()));
                }

                if (const std::shared_ptr<Session>& session = weak.lock())
                {
                    session->Close();
                }

            }, shared_from_this(), session->GetId(), _session);
    }

    void LobbyServerSessionStateMachine::Shutdown()
    {
        _shutdown.store(true);

        _channel->Close();
    }

    void LobbyServerSessionStateMachine::Receive(Buffer buffer)
    {
        _channel->Send(std::move(buffer), channel::ChannelSignal::NotifyOne);
    }

    auto LobbyServerSessionStateMachine::GetName() const -> std::string_view
    {
        return _name;
    }

    auto LobbyServerSessionStateMachine::Run() -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        AsyncEnumerable<Buffer> enumerable(_channel);

        Buffer receiveBuffer;

        while (enumerable.HasNext())
        {
            Buffer buffer = co_await enumerable;

            const std::shared_ptr<Session>& session = _session.lock();

            if (_shutdown.load() || !session)
            {
                co_return;
            }

            assert(ExecutionContext::IsEqualTo(session->GetStrand()));

            try
            {
                receiveBuffer.MergeBack(std::move(buffer));

                if (receiveBuffer.GetSize() < 2)
                {
                    continue;
                }

                PacketReader reader(receiveBuffer.cbegin(), receiveBuffer.cend());

                const int64_t packetSize = reader.Read<int16_t>();
                if (receiveBuffer.GetSize() < packetSize)
                {
                    continue;
                }

                std::unique_ptr<IPacket> packet = network::lobby::cs::CreateFrom(reader);

                Buffer temp;
                [[maybe_unused]] bool sliced = receiveBuffer.SliceFront(temp, packetSize);
                assert(sliced);

                if (!packet)
                {
                    ZEROSUGAR_LOG_WARN(_serviceLocator,
                        std::format("[{}] unnkown packet. session: {}", GetName(), *session));

                    session->Close();

                    co_return;
                }

                co_await OnEvent(std::move(packet));
            }
            catch (const std::exception& e)
            {
                ZEROSUGAR_LOG_WARN(self->_serviceLocator, std::format("[{}] throws. session: {}, exsception: {}",
                    self->GetName(), *session, e.what()));
            }
        }
    }

    auto LobbyServerSessionStateMachine::GetAccountId() const -> int64_t
    {
        return _accountId;
    }

    void LobbyServerSessionStateMachine::SetAccountId(int64_t accountId)
    {
        _accountId = accountId;
    }
}

namespace zerosugar::xr::lobby
{
    ConnectedState::ConnectedState(LobbyServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session)
        : LobbyServerSessionStateMachine::state_type(LobbySessionState::Connected)
        , _stateMachine(stateMachine)
        , _serviceLocator(serviceLocator)
        , _session(session.weak_from_this())
    {
    }

    auto ConnectedState::OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void>
    {
        [[maybe_unused]]
        auto stateMachine = _stateMachine.shared_from_this();

        std::shared_ptr<Session> session = _session.lock();
        if (!session)
        {
            co_return;
        }

        using namespace network::lobby;

        switch (inPacket->GetOpcode())
        {
        case cs::Authenticate::opcode:
        {
            const cs::Authenticate* packet = inPacket->Cast<cs::Authenticate>();
            assert(packet);

            service::ILoginService& loginService = _serviceLocator.Get<service::ILoginService>();

            service::AuthenticateParam param;
            param.token = packet->authenticationToken;

            service::AuthenticateResult result = co_await loginService.AuthenticateAsync(std::move(param));
            if (result.errorCode != service::LoginServiceErrorCode::LoginErrorNone)
            {
                ZEROSUGAR_LOG_WARN(_serviceLocator,
                    std::format("[lobby_session_connected_state] fail to authenticate. session: {}", *session));

                session->Close();
            }

            _stateMachine.SetAccountId(result.accountId);
            _stateMachine.Transition(LobbySessionState::Authenticated);

            co_return;
        }
        break;
        }

        throw std::runtime_error(std::format("unhandled packet. opcode: {}", inPacket->GetOpcode()));
    }

    AuthenticatedState::AuthenticatedState(LobbyServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session)
        : LobbyServerSessionStateMachine::state_type(LobbySessionState::Authenticated)
        , _stateMachine(stateMachine)
        , _serviceLocator(serviceLocator)
        , _session(session.weak_from_this())
    {
    }

    void AuthenticatedState::OnEnter()
    {
        std::shared_ptr<Session> session = _session.lock();
        if (!session)
        {
            return;
        }

        service::GetLobbyCharactersParam param;
        param.accountId = _stateMachine.GetAccountId();

        _serviceLocator.Get<service::IDatabaseService>().GetLobbyCharactersAsync(std::move(param))
            .Then(session->GetStrand(), [session](service::GetLobbyCharactersResult result)
                {
                    if (result.errorCode == service::DatabaseServiceErrorCode::DatabaseErrorNone)
                    {
                        const auto notifyCharacterList = [&result]() -> network::lobby::sc::NotifyCharacterList
                            {
                                network::lobby::sc::NotifyCharacterList notify;
                                notify.count = static_cast<int32_t>(result.lobbyCharacters.size());

                                for (const service::DTOLobbyCharacter& dto : result.lobbyCharacters)
                                {
                                    network::lobby::LobbyCharacter& character = notify.character.emplace_back();
                                    character.slot = dto.slot;
                                    character.name = dto.name;
                                    character.level = dto.level;
                                    character.str = dto.str;
                                    character.dex = dto.dex;
                                    character.intell = dto.intell;
                                    character.job = dto.job;
                                    character.faceId = dto.faceId;
                                    character.hairId = dto.hairId;
                                    character.zoneId = dto.zoneId;

                                    for (const service::DTOLobbyItem& item : dto.items)
                                    {
                                        const auto pos = static_cast<data::EquipPosition>(item.equipPosition);
                                        switch (pos)
                                        {
                                        case data::EquipPosition::Armor:
                                            character.armorId = item.itemDataId;
                                            break;
                                        case data::EquipPosition::Gloves:
                                            character.glovesId = item.itemDataId;
                                            break;
                                        case data::EquipPosition::Shoes:
                                            character.shoesId = item.itemDataId;
                                            break;
                                        case data::EquipPosition::Weapon:
                                            character.weaponId = item.itemDataId;
                                            break;
                                        default:
                                            assert(false);
                                        }
                                    }
                                }

                                return notify;
                            }();

                        session->Send(PacketBuilder::MakePacket(notifyCharacterList));
                    }
                    else
                    {
                        session->Close();
                    }
                });
    }

    auto AuthenticatedState::OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void>
    {
        using namespace network::lobby;

        std::shared_ptr<Session> session = _session.lock();
        if (!session)
        {
            co_return;
        }

        switch (inPacket->GetOpcode())
        {
        case cs::CreateCharacter::opcode:
        {
            const cs::CreateCharacter* packet = inPacket->Cast<cs::CreateCharacter>();
            assert(packet);

            co_await HandlePacket(*session, *packet);
        }
        break;
        case cs::DeleteCharacter::opcode:
        {
            const cs::DeleteCharacter* packet = inPacket->Cast<cs::DeleteCharacter>();
            assert(packet);

            co_await HandlePacket(*session, *packet);
        }
        break;
        case cs::SelectCharacter::opcode:
        {
            const cs::SelectCharacter* packet = inPacket->Cast<cs::SelectCharacter>();
            assert(packet);

            co_await HandlePacket(*session, *packet);
        }
        break;

        }

        co_return;
    }

    auto AuthenticatedState::HandlePacket(Session& session, const network::lobby::cs::CreateCharacter& packet) -> Future<void>
    {
        service::IDatabaseService& databaseService = _serviceLocator.Get<service::IDatabaseService>();

        service::AddCharacterParam param;
        {
            service::DTOCharacterAdd& addParam = param.characterAdd;
            const network::lobby::LobbyCharacter& request = packet.character;

            addParam.accountId = _stateMachine.GetAccountId();
            addParam.slot = request.slot;
            addParam.name = request.name;
            addParam.level = request.level;
            addParam.str = request.str;
            addParam.dex = request.dex;
            addParam.intell = request.intell;
            addParam.job = request.job;
            addParam.faceId = request.faceId;
            addParam.hairId = request.hairId;
            addParam.gold = request.gold;
            addParam.zoneId = request.zoneId;
            addParam.x = request.x;
            addParam.y = request.y;
            addParam.z = request.z;
        }
        {
            service::DTOEquipItem equipItem;
            equipItem.item.itemId = 0;
            equipItem.item.itemDataId = packet.character.armorId;
            equipItem.item.option = service::DTOItemOption{};
        }

        service::AddCharacterResult result = co_await databaseService.AddCharacterAsync(std::move(param));
        if (result.errorCode != service::DatabaseServiceErrorCode::DatabaseErrorNone)
        {
            network::lobby::sc::ResultCreateCharacter outPacket;
            outPacket.success = false;

            session.Send(PacketBuilder::MakePacket(outPacket));

            co_return;
        }

    }

    auto AuthenticatedState::HandlePacket(Session& session, const network::lobby::cs::DeleteCharacter& packet) -> Future<void>
    {
        (void)session;
        (void)packet;

        co_return;

        //service::IDatabaseService& databaseService = _serviceLocator.Get<service::IDatabaseService>();
    }

    auto AuthenticatedState::HandlePacket(Session& session, const network::lobby::cs::SelectCharacter& packet) -> Future<void>
    {
        (void)session;
        (void)packet;

        co_return;

        //service::IDatabaseService& databaseService = _serviceLocator.Get<service::IDatabaseService>();
    }

    TransitionToGameState::TransitionToGameState()
        : LobbyServerSessionStateMachine::state_type(LobbySessionState::TransitionToGame)
    {
    }

    void TransitionToGameState::OnEnter()
    {
        IState<LobbySessionState, StateEvent<std::unique_ptr<IPacket>, Future<void>>>::OnEnter();
    }

    auto TransitionToGameState::OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void>
    {
        (void)inPacket;

        co_return;
    }
}
