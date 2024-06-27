#pragma once

namespace zerosugar::xr
{
    class GameEntity;
    class IPacket;
}

namespace zerosugar::xr
{
    class IGameEntityController
    {
    public:
        virtual ~IGameEntityController() = default;

        virtual bool IsSubscriberOf(int32_t opcode) const = 0;
        virtual void Notify(const IPacket& packet) = 0;

        virtual auto GetControllerId() const -> int64_t = 0;
        virtual void SetControllerId(int64_t id) = 0;
    };
}
