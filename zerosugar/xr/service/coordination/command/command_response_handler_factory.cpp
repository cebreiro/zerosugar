#include "command_response_handler_factory.h"

#include "zerosugar/xr/service/coordination/command/command_response_handler.hpp"

namespace zerosugar::xr::coordination
{
    CommandResponseHandlerFactory::CommandResponseHandlerFactory()
    {
        Register<LaunchGameInstanceResponseHandler>();
    }

    CommandResponseHandlerFactory::~CommandResponseHandlerFactory()
    {
    }

    auto CommandResponseHandlerFactory::CreateHandler(int32_t opcode) const
        -> std::shared_ptr<ICommandResponseHandler>
    {
        auto iter = _factoryFunctions.find(opcode);

        return iter != _factoryFunctions.end() ? iter->second() : nullptr;
    }

    template <typename T> requires std::derived_from<T, ICommandResponseHandler>
    void CommandResponseHandlerFactory::Register()
    {
        const int32_t opcode = T::opcode;

        [[maybe_unused]]
        const bool inserted = _factoryFunctions.emplace(opcode, []() -> std::shared_ptr<ICommandResponseHandler>
            {
                static T instance;

                return std::shared_ptr<ICommandResponseHandler>(
                    std::shared_ptr<ICommandResponseHandler>(), &instance);

            }).second;

        assert(inserted);
    }
}
