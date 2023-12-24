#pragma once

namespace zerosugar::sl
{
    class ILoginPacketHandler;

    class LoginPacketHandlerContainer
    {
        LoginPacketHandlerContainer();

    public:
        static auto GetInstance() -> const LoginPacketHandlerContainer&;

        auto Find(int8_t value) const -> const ILoginPacketHandler*;

    private:
        template <typename T>
        void Register();

        bool Register(int8_t value, const ILoginPacketHandler* handler);

    private:
        std::unordered_map<int8_t, const ILoginPacketHandler*> _handlers;
    };

    template <typename T>
    void LoginPacketHandlerContainer::Register()
    {
        static const T handler;
        [[maybe_unused]] const bool result = Register(handler.GetOpcode(), &handler);

        assert(result);
    }
}
