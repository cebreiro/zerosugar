#pragma once
#include <cassert>
#include <unordered_map>

namespace zerosugar::sl
{
    class ILoginPacketHandler;

    class LoginPacketHandlerContainer
    {
        LoginPacketHandlerContainer();

    public:
        template <typename T>
        struct Registry
        {
            Registry();
        };

    public:
        static auto GetInstance() -> LoginPacketHandlerContainer&;

        auto Find(int8_t value) -> const ILoginPacketHandler*;

    private:
        template <typename T>
        void Register();

        bool Register(int8_t value, const ILoginPacketHandler* handler);

    private:
        std::unordered_map<int8_t, const ILoginPacketHandler*> _handlers;
    };

    template <typename T>
    LoginPacketHandlerContainer::Registry<T>::Registry()
    {
        static const T handler;
        [[maybe_unused]] const bool result = GetInstance().Register(handler.GetOpcode(), &handler);

        assert(result);
    }

    template <typename T>
    void LoginPacketHandlerContainer::Register()
    {
        static const T handler;
        [[maybe_unused]] const bool result = Register(handler.GetOpcode(), &handler);

        assert(result);
    }
}
