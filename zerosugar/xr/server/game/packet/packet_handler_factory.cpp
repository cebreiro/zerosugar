#include "packet_handler_factory.h"

#include "zerosugar/xr/server/game/packet/packet_handler.hpp"

namespace zerosugar::xr
{
    GamePacketHandlerFactory::GamePacketHandlerFactory()
    {
        Register<AuthenticateHandler>();
        Register<LoadLevelCompleteHandler>();
        Register<MovePlayerHandler>();
        Register<StopPlayerHandler>();
        Register<SprintPlayerHandler>();
        Register<RollDodgePlayerHandler>();
        Register<ChatHandler>();
        Register<SwapItemHandler>();
        Register<StartDungeonMatchHandler>();
        Register<CancelDungeonMatchHandler>();
        Register<ApproveDungeonMatchHandler>();
        Register<RejectDungeonMatchHandler>();
    }

    GamePacketHandlerFactory::~GamePacketHandlerFactory()
    {
    }

    auto GamePacketHandlerFactory::CreateHandler(int32_t opcode) const -> std::shared_ptr<IGamePacketHandler>
    {
        auto iter = _factoryFunctions.find(opcode);

        return iter != _factoryFunctions.end() ? iter->second() : nullptr;
    }

    template <typename T> requires std::derived_from<T, IGamePacketHandler>
    void GamePacketHandlerFactory::Register()
    {
        const int32_t opcode = T::opcode;

        [[maybe_unused]]
        const bool inserted = _factoryFunctions.emplace(opcode, []() -> std::shared_ptr<IGamePacketHandler>
            {
                static T instance;

                return std::shared_ptr<IGamePacketHandler>(
                    std::shared_ptr<IGamePacketHandler>(), &instance);

            }).second;

        assert(inserted);
    }
}
