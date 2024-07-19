#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameEntity;
    class GameExecutionParallel;
    class GameExecutionSerial;
}

namespace zerosugar::xr
{
    template <typename T>
    concept game_task_target_selector_concept = requires (T t, const GameExecutionParallel& parallel, const GameExecutionSerial& serial)
    {
        typename T::target_type;

        { t.SelectEntityId(serial) } -> std::same_as<bool>;
        { t.GetTargetId() } -> std::same_as<std::span<const game_entity_id_type>>;

        { t.SelectEntity(parallel) } -> std::same_as<bool>;
        { static_cast<const T&>(t).GetTarget() } -> std::same_as<typename T::target_type>;
    };
}

namespace zerosugar::xr::game_task
{
    struct DummyTarget{};

    class NullSelector
    {
    public:
        using target_type = DummyTarget;

        bool SelectEntityId(const GameExecutionSerial& serial);
        auto GetTargetId() const -> std::span<const game_entity_id_type>;

        bool SelectEntity(const GameExecutionParallel& parallel);
        auto GetTarget() const -> target_type;

    private:
        game_entity_id_type _id = game_entity_id_type::Null();
    };

    class MainTargetSelector
    {
    public:
        using target_type = PtrNotNull<GameEntity>;

        explicit MainTargetSelector(game_entity_id_type targetId);

        bool SelectEntityId(const GameExecutionSerial& serial);
        auto GetTargetId() const -> std::span<const game_entity_id_type>;

        bool SelectEntity(const GameExecutionParallel& parallel);
        auto GetTarget() const -> target_type;

    private:
        game_entity_id_type _mainTargetId;
        std::shared_ptr<GameEntity> _entity;
    };

    class MultiTargetSelector
    {
    public:
        using target_type = const std::vector<PtrNotNull<GameEntity>>&;

    public:
        template <typename R> requires std::is_same_v<std::ranges::range_value_t<R>, game_entity_id_type>
        explicit MultiTargetSelector(R&& range)
            : _targetIds(std::ranges::to<std::vector<game_entity_id_type>>(std::forward<R>(range)))
        {
        }

        bool SelectEntityId(const GameExecutionSerial& serial);
        auto GetTargetId() const -> std::span<const game_entity_id_type>;

        bool SelectEntity(const GameExecutionParallel& parallel);
        auto GetTarget() const -> target_type;

    private:
        std::vector<game_entity_id_type> _targetIds;
        std::vector<PtrNotNull<GameEntity>> _targets;
    };

    class PlayerAttackEffectTargetSelector
    {
    public:
        using target_type = const std::vector<PtrNotNull<GameEntity>>&;

    public:
        PlayerAttackEffectTargetSelector(game_entity_id_type playerId, std::span<const game_entity_id_type> targetIds);

        bool SelectEntityId(const GameExecutionSerial& serial);
        auto GetTargetId() const -> std::span<const game_entity_id_type>;

        bool SelectEntity(const GameExecutionParallel& parallel);
        auto GetTarget() const -> target_type;

    private:
        game_entity_id_type _playerId;
        std::vector<game_entity_id_type> _targetIds;
        std::vector<PtrNotNull<GameEntity>> _targets;
    };

    class BoxSkillTargetSelector
    {
    public:
        using target_type = const std::vector<PtrNotNull<GameEntity>>&;

    public:
        BoxSkillTargetSelector(const Eigen::Vector3d& center, const Eigen::AlignedBox2d& box, float yaw, GameEntityType targetType);

        bool SelectEntityId(const GameExecutionSerial& serial);
        auto GetTargetId() const -> std::span<const game_entity_id_type>;

        bool SelectEntity(const GameExecutionParallel& parallel);
        auto GetTarget() const -> target_type;

    private:
        Eigen::Vector3d _center;
        Eigen::AlignedBox2d _box;
        float _yaw = 0.f;
        GameEntityType _targetType = GameEntityType::Player;

        std::vector<game_entity_id_type> _targetIds;
        std::vector<PtrNotNull<GameEntity>> _targets;
    };

    static_assert(game_task_target_selector_concept<NullSelector>);
    static_assert(game_task_target_selector_concept<MainTargetSelector>);
}
