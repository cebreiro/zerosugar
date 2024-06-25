#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameEntity;
    class GameInstance;

    template <typename T>
    concept game_task_target_selector_concept = requires (T t, GameInstance& gameInstance)
    {
        typename T::target_type;

        { t.SelectEntityId(static_cast<const GameInstance&>(gameInstance)) } -> std::same_as<bool>;
        { t.GetTargetId() } -> std::same_as<std::span<const game_entity_id_type>>;

        { t.SelectEntity(static_cast<const GameInstance&>(gameInstance)) } -> std::same_as<bool>;
        { static_cast<const T&>(t).GetTarget() } -> std::same_as<typename T::target_type>;
    };
}

namespace zerosugar::xr::game_task
{
    class MainTargetSelector
    {
    public:
        using target_type = GameEntity*;

        explicit MainTargetSelector(game_entity_id_type targetId);

        bool SelectEntityId(const GameInstance& gameInstance);
        void GetTargetId() const -> std::span<const game_entity_id_type>;

        bool SelectEntity(const GameInstance& gameInstance);
        auto GetTarget() const -> target_type;

    private:
        game_entity_id_type _mainTargetId;
        std::shared_ptr<GameEntity> _entity;
    };

    static_assert(game_task_target_selector_concept<MainTargetSelector>);
}
