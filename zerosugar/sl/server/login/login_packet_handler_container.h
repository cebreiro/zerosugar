#pragma once

namespace zerosugar::sl
{
    class LoginServer;
    class ILoginPacketHandler;

    class LoginPacketHandlerContainer
    {
    public:
        LoginPacketHandlerContainer() = delete;
        explicit LoginPacketHandlerContainer(LoginServer& server);
        ~LoginPacketHandlerContainer();

        auto Find(int8_t value) const -> const ILoginPacketHandler*;

    private:
        template <typename T>
        void Add(LoginServer& server);
        bool Add(int8_t value, UniquePtrNotNull<const ILoginPacketHandler> handler);

    private:
        std::unordered_map<int8_t, UniquePtrNotNull<const ILoginPacketHandler>> _handlers;
    };
}
