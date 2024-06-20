#include "command_handler_factory.h"

namespace zerosugar::xr
{
    CommandHandlerFactory::CommandHandlerFactory()
    {
    }

    CommandHandlerFactory::~CommandHandlerFactory()
    {
    }

    auto CommandHandlerFactory::CreateHandler(int32_t opcode) const -> std::shared_ptr<ICommandHandler>
    {
        auto iter = _factoryFunctions.find(opcode);

        return iter != _factoryFunctions.end() ? iter->second() : nullptr;
    }

    template <typename T> requires std::derived_from<T, ICommandHandler>
    void CommandHandlerFactory::Register()
    {
        const int32_t opcode = T::opcode;

        [[maybe_unused]]
        const bool inserted = _factoryFunctions.emplace(opcode, []() -> std::shared_ptr<ICommandHandler>
            {
                static T instance;

                return std::shared_ptr<ICommandHandler>(
                    std::shared_ptr<ICommandHandler>(), &instance);

            }).second;

        assert(inserted);
    }
}
