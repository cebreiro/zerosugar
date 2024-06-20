#pragma once

namespace zerosugar::xr::coordination
{
    class ICommandResponseHandler;

    class ICommandResponseHandlerFactory
    {
    public:
        virtual ~ICommandResponseHandlerFactory() = default;

        virtual auto CreateHandler(int32_t opcode) const -> std::shared_ptr<ICommandResponseHandler> = 0;
    };
}
