#pragma once

namespace zerosugar::sl
{
    class IGatewayPacketHandler;

    class GatewayPacketHandlerContainer
    {
        GatewayPacketHandlerContainer();

    public:
        static auto GetInstance() -> const GatewayPacketHandlerContainer&;

        auto Find(int32_t value) const -> const IGatewayPacketHandler*;

    private:
        template <typename T>
        void Register();

        bool Register(int32_t value, const IGatewayPacketHandler* handler);

    private:
        std::unordered_map<int32_t, const IGatewayPacketHandler*> _handlers;
    };

    template <typename T>
    void GatewayPacketHandlerContainer::Register()
    {
        static const T handler;
        [[maybe_unused]] const bool result = Register(handler.GetOpcode(), &handler);

        assert(result);
    }
}
