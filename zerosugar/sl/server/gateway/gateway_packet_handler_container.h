#pragma once

namespace zerosugar::sl
{
    class GatewayServer;
    class IGatewayPacketHandler;

    class GatewayPacketHandlerContainer
    {
    public:
        GatewayPacketHandlerContainer() = delete;
        explicit GatewayPacketHandlerContainer(GatewayServer& server);
        ~GatewayPacketHandlerContainer();

        auto Find(int32_t value) const -> const IGatewayPacketHandler*;

    private:
        template <typename T>
        void Add(GatewayServer& server);
        bool Add(int32_t value, UniquePtrNotNull<const IGatewayPacketHandler> handler);

    private:
        std::unordered_map<int32_t, UniquePtrNotNull<const IGatewayPacketHandler>> _handlers;
    };
}
