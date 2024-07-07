#pragma once

namespace zerosugar::xr
{
    class IGamePacketHandler;
}

namespace zerosugar::xr
{
    class IGamePacketHandlerFactory
    {
    public:
        virtual ~IGamePacketHandlerFactory() = default;

        virtual auto CreateHandler(int32_t opcode) const -> std::shared_ptr<IGamePacketHandler> = 0;
    };
}
