#include "lobby_session_state.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/shared/snowflake/snowflake.h"
#include "zerosugar/xr/data/enum/equip_position.h"
#include "zerosugar/xr/network/packet_builder.h"
#include "zerosugar/xr/network/model/generated/lobby_cs_message.h"
#include "zerosugar/xr/network/model/generated/lobby_sc_message.h"
#include "zerosugar/xr/network/model/generated/lobby_cs_message_json.h"

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
            _stateMachine.SetAuthenticationToken(packet->authenticationToken);

            _stateMachine.Transition(LobbySessionState::Authenticated);

            co_return;
        }
        break;
        }

        throw std::runtime_error(std::format("unhandled packet. opcode: {}", inPacket->GetOpcode()));
    }

    AuthenticatedState::AuthenticatedState(LobbyServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, IUniqueIDGenerator& idGenerator, Session& session)
        : LobbyServerSessionStateMachine::state_type(LobbySessionState::Authenticated)
        , _stateMachine(stateMachine)
        , _serviceLocator(serviceLocator)
        , _idGenerator(idGenerator)
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

        Promise<void> promise;
        _pendingGetCharacterList = promise.GetFuture();

        service::GetLobbyCharactersParam param;
        param.accountId = _stateMachine.GetAccountId();

        _serviceLocator.Get<service::IDatabaseService>().GetLobbyCharactersAsync(std::move(param))
            .Then(session->GetStrand(),
                [session, p = std::move(promise), self = shared_from_this()](service::GetLobbyCharactersResult result) mutable
                {
                    if (result.errorCode == service::DatabaseServiceErrorCode::DatabaseErrorNone)
                    {
                        const auto notifyCharacterList = [&result, &self]() -> network::lobby::sc::NotifyCharacterList
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

                                    self->AddCharacter(dto.slot, CharacterCache{
                                        .characterId = dto.characterId,
                                        .zoneId = dto.zoneId
                                    });
                                }

                                return notify;
                            }();

                        session->Send(PacketBuilder::MakePacket(notifyCharacterList));
                    }
                    else
                    {
                        session->Close();
                    }

                    p.Set();
                });
    }

    auto AuthenticatedState::OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void>
    {
        if (_pendingGetCharacterList.IsPending())
        {
            co_await _pendingGetCharacterList;
        }

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

    bool AuthenticatedState::HasCharacter(int32_t slotId) const
    {
        return _characters.contains(slotId);
    }

    auto AuthenticatedState::FindCharacter(int32_t slotId) const -> const CharacterCache*
    {
        const auto iter = _characters.find(slotId);

        return iter != _characters.end() ? &iter->second : nullptr;
    }

    void AuthenticatedState::AddCharacter(int32_t slotId, const CharacterCache& character)
    {
        [[maybe_unused]]
        bool inserted = _characters.try_emplace(slotId, character).second;
        assert(inserted);
    }

    void AuthenticatedState::RemoveCharacter(int32_t slotId)
    {
        [[maybe_unused]]
        const size_t erased = _characters.erase(slotId);
        assert(erased > 0);
    }

    auto AuthenticatedState::HandlePacket(Session& session, const network::lobby::cs::CreateCharacter& packet) -> Future<void>
    {
        if (HasCharacter(packet.character.slot))
        {
            ZEROSUGAR_LOG_WARN(_serviceLocator,
                std::format("[lobby_stete_authenticated] invalid request - character slot already used. session: {}, packet: {}",
                    session, nlohmann::json(packet).dump()));

            network::lobby::sc::ResultCreateCharacter outPacket;
            outPacket.success = false;

            session.Send(PacketBuilder::MakePacket(outPacket));

            co_return;
        }

        service::IDatabaseService& databaseService = _serviceLocator.Get<service::IDatabaseService>();

        service::AddCharacterParam param;
        {
            service::DTOCharacterAdd& addParam = param.characterAdd;
            const network::lobby::LobbyCharacter& request = packet.character;

            addParam.accountId = _stateMachine.GetAccountId();
            addParam.slot = request.slot;
            addParam.name = request.name;
            addParam.level = 1;
            addParam.str = 10;
            addParam.dex = 10;
            addParam.intell = 10;
            addParam.job = request.job;
            addParam.faceId = request.faceId;
            addParam.hairId = request.hairId;
            addParam.gold = 0;
            addParam.zoneId = 100;
            addParam.x = 0;
            addParam.y = 0;
            addParam.z = 0;
        }
        {
            constexpr std::array equipItems{
                std::pair{ 3000001, data::EquipPosition::Armor },
                std::pair{ 3100001, data::EquipPosition::Gloves },
                std::pair{ 3200001, data::EquipPosition::Shoes },
                std::pair{ 3300001, data::EquipPosition::Weapon },
            };

            for (const auto& [itemId, equipPosition] : equipItems)
            {
                service::DTOEquipItem& equipItem = param.equipItems.emplace_back();
                equipItem.item.itemId = static_cast<int64_t>(_idGenerator.Generate());
                equipItem.item.itemDataId = itemId;
                equipItem.equipPosition = static_cast<int32_t>(equipPosition);
            }
        }

        service::AddCharacterResult result = co_await databaseService.AddCharacterAsync(param);

        network::lobby::sc::ResultCreateCharacter outPacket;
        outPacket.success = result.errorCode == service::DatabaseServiceErrorCode::DatabaseErrorNone;

        if (outPacket.success)
        {
            AddCharacter(param.characterAdd.slot, CharacterCache{
                .characterId = result.characterId,
                .zoneId = param.characterAdd.zoneId,
            });

            outPacket.character.slot = param.characterAdd.slot;
            outPacket.character.name = param.characterAdd.name;
            outPacket.character.level = param.characterAdd.level;
            outPacket.character.str = param.characterAdd.str;
            outPacket.character.dex = param.characterAdd.dex;
            outPacket.character.intell = param.characterAdd.intell;
            outPacket.character.job = param.characterAdd.job;
            outPacket.character.faceId = param.characterAdd.faceId;
            outPacket.character.hairId = param.characterAdd.hairId;
            outPacket.character.gold = param.characterAdd.gold;
            outPacket.character.zoneId = param.characterAdd.zoneId;
            outPacket.character.x = param.characterAdd.x;
            outPacket.character.y = param.characterAdd.y;
            outPacket.character.z = param.characterAdd.z;
        }

        session.Send(PacketBuilder::MakePacket(outPacket));
    }

    auto AuthenticatedState::HandlePacket(Session& session, const network::lobby::cs::DeleteCharacter& packet) -> Future<void>
    {
        const auto* character = FindCharacter(packet.slot);
        if (!character)
        {
            ZEROSUGAR_LOG_WARN(_serviceLocator,
                std::format("[lobby_stete_authenticated] invalid request - character slot is empty. session: {}, slot: {}",
                    session, packet.slot));

            co_return;
        }

        service::IDatabaseService& databaseService = _serviceLocator.Get<service::IDatabaseService>();

        service::RemoveCharacterParam param;
        param.characterId = character->characterId;

        const service::RemoveCharacterResult& result = co_await databaseService.RemoveCharacterAsync(std::move(param));

        if (result.errorCode != service::DatabaseServiceErrorCode::DatabaseErrorNone)
        {
            co_return;
        }

        RemoveCharacter(packet.slot);

        network::lobby::sc::SuccessDeleteCharacter outPacket;
        outPacket.slot = packet.slot;

        session.Send(PacketBuilder::MakePacket(outPacket));

        co_return;
    }

    auto AuthenticatedState::HandlePacket(Session& session, const network::lobby::cs::SelectCharacter& packet) -> Future<void>
    {
        (void)session;

        do
        {
            const CharacterCache* character = FindCharacter(packet.slot);
            if (!character)
            {
                break;
            }

            service::ICoordinationService& service = _serviceLocator.Get<service::ICoordinationService>();

            service::AddPlayerParam param;
            param.authenticationToken = _stateMachine.GetAuthenticationToken();
            param.accountId = _stateMachine.GetAccountId();
            param.characterId = character->characterId;
            param.zoneId = character->zoneId;

            service::AddPlayerResult result = co_await service.AddPlayerAsync(std::move(param));

            if (result.errorCode != service::CoordinationServiceErrorCode::CoordinationErrorNone)
            {
                break;
            }

            network::lobby::sc::SuccessSelectCharacter outPacket;
            outPacket.ip = result.ip;
            outPacket.port = result.port;

            session.Send(PacketBuilder::MakePacket(outPacket));

            _stateMachine.Transition(LobbySessionState::TransitionToGame);

            co_return;

        } while (false);

        ZEROSUGAR_LOG_WARN(_serviceLocator,
            std::format("[lobby_stete_authenticated] fail to select character. session: {}, packet: {}",
                session, nlohmann::json(packet).dump()));
    }

    TransitionToGameState::TransitionToGameState(Session& session)
        : LobbyServerSessionStateMachine::state_type(LobbySessionState::TransitionToGame)
        , _session(session.weak_from_this())
    {
    }

    void TransitionToGameState::OnEnter()
    {
        execution::IExecutor* executor = ExecutionContext::GetExecutor();
        assert(executor);

        Delay(std::chrono::seconds(3))
            .Then(*executor, [weak = _session]()
                {
                    const std::shared_ptr<Session>& session = weak.lock();
                    if (session)
                    {
                        session->Close();
                    }
                });
    }

    auto TransitionToGameState::OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void>
    {
        (void)inPacket;

        co_return;
    }
}
