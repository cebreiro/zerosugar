#pragma once

namespace zerosugar::xr
{
    class ICommandHandler;

    class ICommandHandlerFactory
    {
    public:
        virtual ~ICommandHandlerFactory() = default;

        virtual auto CreateHandler(int32_t opcode) const -> std::shared_ptr<ICommandHandler> = 0;
    };
}
