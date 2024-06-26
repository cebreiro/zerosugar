#pragma once
#include "zerosugar/xr/server/game/command/command_handler_factory_interface.h"

namespace zerosugar::xr
{
    class CommandHandlerFactory final : public ICommandHandlerFactory
    {
    public:
        using factory_function_type = std::function<std::shared_ptr<ICommandHandler>()>;

    public:
        CommandHandlerFactory();
        ~CommandHandlerFactory();

        auto CreateHandler(int32_t opcode) const -> std::shared_ptr<ICommandHandler> override;

    private:
        template <typename T> requires std::derived_from<T, ICommandHandler>
        void Register();

    private:
        std::unordered_map<int32_t, factory_function_type> _factoryFunctions;
    };
}
