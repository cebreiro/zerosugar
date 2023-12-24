#include "login_packet_handler_container.h"

#include "zerosugar/sl/server/login/handler/login_packet_handler.h"

namespace zerosugar::sl
{
    LoginPacketHandlerContainer::LoginPacketHandlerContainer()
    {
        Register<detail::LoginPacketHandler_Login>();
        Register<detail::LoginPacketHandler_WorldSelect>();
    }

    auto LoginPacketHandlerContainer::GetInstance() -> const LoginPacketHandlerContainer&
    {
        static const LoginPacketHandlerContainer instance;

        return instance;
    }

    auto LoginPacketHandlerContainer::Find(int8_t value) const -> const ILoginPacketHandler*
    {
        auto iter = _handlers.find(value);
        return iter != _handlers.end() ? iter->second : nullptr;
    }

    bool LoginPacketHandlerContainer::Register(int8_t value, const ILoginPacketHandler* handler)
    {
        return _handlers.try_emplace(value, handler).second;
    }
}
