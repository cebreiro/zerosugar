#pragma once
#include "zerosugar/sl/server/gateway/gateway_client_state.h"

namespace zerosugar::sl
{
    class GatewayClient;
    class GatewayServer;
    class PacketReader;

    class IGatewayPacketHandler
    {
    public:
        IGatewayPacketHandler() = default;
        virtual ~IGatewayPacketHandler() = default;

        virtual bool CanHandle(const GatewayClient& client) const = 0;
        virtual auto Handle(GatewayClient& client, PacketReader& reader) const->Future<void> = 0;

        virtual auto GetOpcode() const -> int32_t = 0;
        virtual auto GetName() const -> std::string_view = 0;
    };
}

namespace zerosugar::sl::detail
{
    class GatewayPacketHandlerAbstract : public IGatewayPacketHandler
    {
    public:
        GatewayPacketHandlerAbstract() = delete;
        explicit GatewayPacketHandlerAbstract(WeakPtrNotNull<GatewayServer> server);

        bool CanHandle(const GatewayClient& client) const final;

    protected:
        void AddAllowedState(GatewayClientState state);
        void AddAllowedStateRange(std::initializer_list<GatewayClientState> states);

    private:
        std::set<GatewayClientState> _allowedStates;

    protected:
        WeakPtrNotNull<GatewayServer> _server;
    };

    template <typename T>
    concept gateway_packet_concept = requires
    {
        requires std::convertible_to<decltype(T::opcode), int32_t>;
    };

    template <gateway_packet_concept T>
    class GatewayPacketHandlerT : public GatewayPacketHandlerAbstract
    {
    public:
        GatewayPacketHandlerT() = delete;
        explicit GatewayPacketHandlerT(WeakPtrNotNull<GatewayServer> server);

        auto GetOpcode() const -> int32_t final;
        auto GetName() const -> std::string_view final;

    private:
        auto Handle(GatewayClient& client, PacketReader& reader) const -> Future<void> final;

    private:
        virtual auto HandlePacket(GatewayServer& server, GatewayClient& client, const T& packet) const -> Future<void> = 0;
    };

    template <gateway_packet_concept T>
    GatewayPacketHandlerT<T>::GatewayPacketHandlerT(WeakPtrNotNull<GatewayServer> server)
        : GatewayPacketHandlerAbstract(std::move(server))
    {
    }

    template <gateway_packet_concept T>
    auto GatewayPacketHandlerT<T>::GetOpcode() const -> int32_t
    {
        return T::opcode;
    }

    template <gateway_packet_concept T>
    auto GatewayPacketHandlerT<T>::GetName() const -> std::string_view
    {
        return typeid(*this).name();
    }

    template <gateway_packet_concept T>
    auto GatewayPacketHandlerT<T>::Handle(GatewayClient& client, PacketReader& reader) const
        -> Future<void>
    {
        const std::shared_ptr<GatewayServer> gatewayServer = _server.lock();
        if (!gatewayServer)
        {
            throw std::runtime_error("fail to handle packet. gateway server is null");
        }

        T packet = {};
        packet.Deserialize(reader);

        co_await this->HandlePacket(*gatewayServer, client, packet);

        co_return;
    }
}
