#pragma once

namespace zerosugar
{
    class Session;
}

namespace zerosugar::xr
{
    class GameServer;
    class IPacket;

    class IGamePacketHandler
    {
    public:
        virtual ~IGamePacketHandler() = default;

        virtual auto Handle(GameServer& server, SharedPtrNotNull<Session> session, UniquePtrNotNull<IPacket> packet) -> Future<void> = 0;
    };

    template <typename T>
    class IGamePacketHandlerT : public IGamePacketHandler
    {
    public:
        static constexpr auto opcode = T::opcode;

    public:
        auto Handle(GameServer& server, SharedPtrNotNull<Session> session, UniquePtrNotNull<IPacket> packet) -> Future<void> final
        {
            const T* casted = packet->Cast<T>();
            assert(casted);

            co_await this->HandlePacket(server, *session, *casted);

            co_return;
        }

    private:
        virtual auto HandlePacket(GameServer& server, Session& session, const T& packet) const -> Future<void> = 0;
    };
}
