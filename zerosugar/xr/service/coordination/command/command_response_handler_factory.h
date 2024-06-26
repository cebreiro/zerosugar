#pragma once
#include "zerosugar/xr/service/coordination/command/command_response_handler_factory_interface.h"

namespace zerosugar::xr::coordination
{
    class CommandResponseHandlerFactory final : public ICommandResponseHandlerFactory
    {
    public:
        using factory_function_type = std::function<std::shared_ptr<ICommandResponseHandler>()>;

    public:
        CommandResponseHandlerFactory();
        ~CommandResponseHandlerFactory();

        auto CreateHandler(int32_t opcode) const -> std::shared_ptr<ICommandResponseHandler> override;

    private:
        template <typename T> requires std::derived_from<T, ICommandResponseHandler>
        void Register();

    private:
        std::unordered_map<int32_t, factory_function_type> _factoryFunctions;
    };
}
