#include "gateway_packet_handler_container.h"

#include "zerosugar/sl/server/gateway/gateway_server.h"
#include "zerosugar/sl/server/gateway/handler/gateway_packet_handler_authentication_request.h"
#include "zerosugar/sl/server/gateway/handler/gateway_packet_handler_interface.h"
#include "zerosugar/sl/server/gateway/handler/gateway_packet_handler_version_check.h"

namespace zerosugar::sl
{
    GatewayPacketHandlerContainer::GatewayPacketHandlerContainer(GatewayServer& server)
    {
        Add<detail::GatewayPacketHandler_VersionCheck>(server);
        Add<detail::GatewayPacketHandler_AuthenticationRequest>(server);
    }

    GatewayPacketHandlerContainer::~GatewayPacketHandlerContainer()
    {
    }

    auto GatewayPacketHandlerContainer::Find(int32_t value) const -> const IGatewayPacketHandler*
    {
        auto iter = _handlers.find(value);
        return iter != _handlers.end() ? iter->second.get() : nullptr;
    }

    template <typename T>
    void GatewayPacketHandlerContainer::Add(GatewayServer& server)
    {
        auto handler = new T(std::static_pointer_cast<GatewayServer>(server.shared_from_this()));
        [[maybe_unused]] const bool result = Add(handler->GetOpcode(), std::unique_ptr<const T>(handler));

        assert(result);
    }

    bool GatewayPacketHandlerContainer::Add(int32_t value, UniquePtrNotNull<const IGatewayPacketHandler> handler)
    {
        return _handlers.try_emplace(value, std::move(handler)).second;
    }
}
