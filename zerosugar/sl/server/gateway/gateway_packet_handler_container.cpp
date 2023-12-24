#include "gateway_packet_handler_container.h"

namespace zerosugar::sl
{
    GatewayPacketHandlerContainer::GatewayPacketHandlerContainer()
    {
    }

    auto GatewayPacketHandlerContainer::GetInstance() -> const GatewayPacketHandlerContainer&
    {
        static const GatewayPacketHandlerContainer instance;

        return instance;
    }

    auto GatewayPacketHandlerContainer::Find(int32_t value) const -> const IGatewayPacketHandler*
    {
        auto iter = _handlers.find(value);
        return iter != _handlers.end() ? iter->second : nullptr;
    }

    bool GatewayPacketHandlerContainer::Register(int32_t value, const IGatewayPacketHandler* handler)
    {
        return _handlers.try_emplace(value, handler).second;
    }
}
