#include "login_packet_handler_container.h"

#include "zerosugar/sl/server/login/login_server.h"
#include "zerosugar/sl/server/login/handler/login_packet_handler.h"

namespace zerosugar::sl
{
    LoginPacketHandlerContainer::LoginPacketHandlerContainer(LoginServer& server)
    {
        Add<detail::LoginPacketHandler_Login>(server);
        Add<detail::LoginPacketHandler_WorldSelect>(server);
    }

    LoginPacketHandlerContainer::~LoginPacketHandlerContainer()
    {
    }

    auto LoginPacketHandlerContainer::Find(int8_t value) const -> const ILoginPacketHandler*
    {
        auto iter = _handlers.find(value);
        return iter != _handlers.end() ? iter->second.get() : nullptr;
    }

    template <typename T>
    void LoginPacketHandlerContainer::Add(LoginServer& server)
    {
        auto handler = new T(std::static_pointer_cast<LoginServer>(server.shared_from_this()));
        [[maybe_unused]] const bool result = Add(handler->GetOpcode(), std::unique_ptr<const T>(handler));

        assert(result);
    }

    bool LoginPacketHandlerContainer::Add(int8_t value, UniquePtrNotNull<const ILoginPacketHandler> handler)
    {
        return _handlers.try_emplace(value, std::move(handler)).second;
    }
}
