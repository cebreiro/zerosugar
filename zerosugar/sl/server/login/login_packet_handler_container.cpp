#include "login_packet_handler_container.h"

#include "zerosugar/sl/server/login/handler/login_packet_handler_login.h"

namespace zerosugar::sl
{
    LoginPacketHandlerContainer::LoginPacketHandlerContainer()
    {
        Register<LoginPacketHandler_Login>();
    }

    auto LoginPacketHandlerContainer::GetInstance() -> LoginPacketHandlerContainer&
    {
        static LoginPacketHandlerContainer instance;

        return instance;
    }

    auto LoginPacketHandlerContainer::Find(int8_t value) -> const ILoginPacketHandler*
    {
        auto iter = _handlers.find(value);
        return iter != _handlers.end() ? iter->second : nullptr;
    }

    bool LoginPacketHandlerContainer::Register(int8_t value, const ILoginPacketHandler* handler)
    {
        return _handlers.try_emplace(value, handler).second;
    }
}
