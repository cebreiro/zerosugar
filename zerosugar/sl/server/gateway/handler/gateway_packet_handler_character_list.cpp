#include "gateway_packet_handler_character_list.h"

#include "zerosugar/sl/protocol/packet/generated/gateway_sc_generated.h"
#include "zerosugar/sl/server/gateway/gateway_client.h"
#include "zerosugar/sl/server/gateway/gateway_server.h"

namespace zerosugar::sl::detail
{
    GatewayPacketHandler_CharacterList::GatewayPacketHandler_CharacterList(WeakPtrNotNull<GatewayServer> server)
        : GatewayPacketHandlerT(std::move(server))
    {
        AddAllowedState(GatewayClientState::Authenticated);
    }

    auto GatewayPacketHandler_CharacterList::HandlePacket(GatewayServer& server, GatewayClient& client,
        const gateway::cs::CharacterListRequest& packet) const -> Future<void>
    {
        auto& serviceLocator = server.GetLocator();
        (void)serviceLocator;

        using namespace service;

        IRepositoryService& repository = server.GetLocator().Get<IRepositoryService>();

        const GetCharacterListParam param{
            .accountId = client.GetAccountId(),
        };
        const GetCharacterListResult& result = co_await repository.GetCharacterListAsync(param);
        if (result.errorCode != RepositoryServiceErrorCode::RepositoryErrorNone)
        {
            ZEROSUGAR_LOG_ERROR(server.GetLocator(),
                std::format("[{}] fail to get character list. client: {}", GetName(), client.ToString()));

            client.Close();
            co_return;
        }

        gateway::sc::CharacterListResponse response;
        response.chractersCount = static_cast<int32_t>(std::ssize(result.character));

        co_return;
    }

    auto GatewayPacketHandler_CharacterList::Convert(const service::Character& character) -> gateway::Character
    {
        (void)character;

        gateway::CharacterAppearance appearance;
        appearance.hatModelId = {};
        appearance.hairColor = {};
        appearance.skinColor = {};
        appearance.face = {};
        appearance.unk01 = {};
        appearance.unk02 = {};
        appearance.hairModelId = {};
        appearance.unk03 = {};
        appearance.unk04 = {};
        appearance.jacketItemId = {};
        appearance.unk05 = {};
        appearance.unk06 = {};
        appearance.glovesModelId = {};
        appearance.unk07 = {};
        appearance.unk08 = {};
        appearance.pantsId = {};
        appearance.unk09 = {};
        appearance.unk10 = {};
        appearance.shoesId = {};
        appearance.unk11 = {};
        appearance.unk12 = {};
        appearance.unk13 = {};
        appearance.unk14 = {};
        appearance.unk15 = {};

        gateway::CharacterInformation information;
        information.name = {};
        information.zone = {};
        information.job = {};
        information.arms = {};
        information.slot = {};
        information.appearanceIfTrueItemIdElseModelId = true;
        information.gender = {};
        information.characterLevel = {};

        gateway::Character result;
        result.appearance = appearance;
        result.information = information;

        return result;
    }
}
