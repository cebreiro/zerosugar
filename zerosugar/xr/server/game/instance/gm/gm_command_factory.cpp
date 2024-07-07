#include "gm_command_factory.h"

#include "zerosugar/xr/server/game/instance/gm/gm_command.hpp"

namespace zerosugar::xr
{
    GMCommandFactory::GMCommandFactory()
    {
        Register<gm::SpawnMonster>();
        Register<gm::VisualizeField>();
    }

    GMCommandFactory::~GMCommandFactory()
    {
    }

    auto GMCommandFactory::CreateHandler(const std::string& command) const -> std::shared_ptr<IGMCommand>
    {
        const auto iter = _factoryFunctions.find(command);

        return iter != _factoryFunctions.end() ? iter->second() : nullptr;
    }

    template <gm_command_concept T> requires std::derived_from<T, IGMCommand>
    void GMCommandFactory::Register()
    {
        [[maybe_unused]]
        const bool inserted = _factoryFunctions.emplace(T::name, []() -> std::shared_ptr<IGMCommand>
            {
                static T instance;

                return std::shared_ptr<IGMCommand>(
                    std::shared_ptr<IGMCommand>(), &instance);

            }).second;

        assert(inserted);
    }
}
