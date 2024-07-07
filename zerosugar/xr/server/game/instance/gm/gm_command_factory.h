#pragma once
#include "zerosugar/xr/server/game/instance/gm/gm_command_factory_interface.h"

namespace zerosugar::xr
{
    template <typename T>
    concept gm_command_concept = requires
    {
        { T::name } -> std::convertible_to<std::string>;
    };

    class GMCommandFactory : public IGMCommandFactory
    {
    public:
    public:
        using factory_function_type = std::function<std::shared_ptr<IGMCommand>()>;

        GMCommandFactory();
        ~GMCommandFactory();

        auto CreateHandler(const std::string& command) const->std::shared_ptr<IGMCommand> override;

    private:
        template <gm_command_concept T> requires std::derived_from<T, IGMCommand>
        void Register();

    private:
        std::unordered_map<std::string, factory_function_type> _factoryFunctions;
    };
}
