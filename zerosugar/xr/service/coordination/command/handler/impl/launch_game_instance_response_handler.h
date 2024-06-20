#pragma once
#include "zerosugar/xr/service/coordination/command/handler/command_response_handler_interface.h"
#include "zerosugar/xr/service/model/generated/coordination_command_response_message_json.h"

namespace zerosugar::xr::coordination
{
    class LaunchGameInstanceResponseHandler final : public CommandResponseHandlerT<command::response::LaunchGameInstance>
    {
    private:
        auto HandleResponse(CoordinationService& service, GameServer& server, const command::response::LaunchGameInstance& response) const -> Future<void> override;
    };
}
