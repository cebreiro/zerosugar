#pragma once
#include "zerosugar/shared/type/runtime_type_id.h"
#include "zerosugar/xr/server/game/instance/component/game_component_id.h"

namespace zerosugar::xr
{
    class GameComponent
    {
    public:
        virtual ~GameComponent() = default;

        template <typename T> requires std::derived_from<T, GameComponent>
        static auto GetClassId() -> game_component_id_type;
    };

    template <typename T> requires std::derived_from<T, GameComponent>
    auto GameComponent::GetClassId() -> game_component_id_type
    {
        using value_type = game_component_id_type::value_type;
        const int64_t value = RuntimeTypeId<GameComponent>::Get<T>();

        return game_component_id_type(value);
    }
}
