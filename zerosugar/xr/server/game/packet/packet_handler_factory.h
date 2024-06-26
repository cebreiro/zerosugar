#pragma once
#include "zerosugar/xr/server/game/packet/packet_handler_factory_interface.h"

namespace zerosugar::xr
{
    class GamePacketHandlerFactory final : public IGamePacketHandlerFactory
    {
    public:
        using factory_function_type = std::function<std::shared_ptr<IGamePacketHandler>()>;

        GamePacketHandlerFactory();
        ~GamePacketHandlerFactory();

        auto CreateHandler(int32_t opcode) const -> std::shared_ptr<IGamePacketHandler> override;

    private:
        template <typename T> requires std::derived_from<T, IGamePacketHandler>
        void Register();

    private:
        std::unordered_map<int32_t, factory_function_type> _factoryFunctions;
    };
}
