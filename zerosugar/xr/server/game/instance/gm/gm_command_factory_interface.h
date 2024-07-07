#pragma once

namespace zerosugar::xr
{
    class IGMCommand;
}

namespace zerosugar::xr
{
    class IGMCommandFactory
    {
    public:
        virtual ~IGMCommandFactory() = default;

        virtual auto CreateHandler(const std::string& command) const -> std::shared_ptr<IGMCommand> = 0;
    };
}
