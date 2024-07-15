#include "dungeon_match.h"

#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/bot_control_service.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"

namespace zerosugar::xr::bot
{
    auto DungeonMatch::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);

        using namespace network::game;

        while (true)
        {
            cs::StartDungeonMatch start;
            start.deugeonId = _mapId;


            controller.SendToServer(Packet::ToBuffer(start));

            const auto event = bt::Event<sc::NotifyDungeonMatchGroupCreation, sc::NotifyDungeonMatchFailure>();
            if (const auto va = co_await event;
                std::holds_alternative<sc::NotifyDungeonMatchFailure>(va))
            {
                ZEROSUGAR_LOG_ERROR(controller.GetServiceLocator(),
                    fmt::format("[{}] [{}] dungon group creation fail!!", controller.GetName(), GetName()));

                continue;
            }


            controller.SendToServer(Packet::ToBuffer(cs::ApproveDungeonMatch{}));

            const auto va = co_await bt::Event<sc::NotifyDungeonMatchGroupApproved, sc::NotifyDungeonMatchFailure>();
            if (std::holds_alternative<sc::NotifyDungeonMatchFailure>(va))
            {
                ZEROSUGAR_LOG_INFO(controller.GetServiceLocator(),
                    fmt::format("[{}] [{}] dungon group rejected", controller.GetName(), GetName()));

                continue;
            }

            const auto& approved = std::get<sc::NotifyDungeonMatchGroupApproved>(va);

            [[maybe_unused]]
            const bool success = blackBoard.Insert("game_dungeon_address", std::pair<std::string, int32_t>(approved.ip, approved.port));
            assert(success);

            co_return true;
        }
    }

    auto DungeonMatch::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(DungeonMatch& self, const pugi::xml_node& xmlNode)
    {
        if (const auto attr = xmlNode.attribute("map_id"); attr)
        {
            self._mapId = attr.as_int();
        }
        else
        {
            assert(false);
        }
    }
}
