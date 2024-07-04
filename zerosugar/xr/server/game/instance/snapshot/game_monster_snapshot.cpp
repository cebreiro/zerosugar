#include "game_monster_snapshot.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"

namespace zerosugar::xr
{
    GameMonsterSnapshot::GameMonsterSnapshot(IGameController& controller, game_entity_id_type id, int32_t dataId)
        : _controller(controller)
        , _id(id)
        , _dataId(dataId)
    {
    }

    void GameMonsterSnapshot::Initialize(const GameEntity& entity)
    {
        const MovementComponent& movementComponent = entity.GetComponent<MovementComponent>();
        const StatComponent& statComponent = entity.GetComponent<StatComponent>();

        _position = movementComponent.GetPosition();

        _hp = statComponent.GetHP().As<float>();
        _maxHP = statComponent.GetMaxHP().As<float>();
    }

    auto GameMonsterSnapshot::GetController() const -> IGameController&
    {
        return _controller;
    }

    auto GameMonsterSnapshot::GetId() const -> game_entity_id_type
    {
        return _id;
    }

    auto GameMonsterSnapshot::GetDataId() const -> int32_t
    {
        return _dataId;
    }

    auto GameMonsterSnapshot::GetPosition() const -> const Eigen::Vector3d&
    {
        return _position;
    }

    auto GameMonsterSnapshot::GetYaw() const -> float
    {
        return _yaw;
    }

    auto GameMonsterSnapshot::GetHp() const -> float
    {
        return _hp;
    }

    auto GameMonsterSnapshot::GetMaxHp() const -> float
    {
        return _maxHP;
    }

    auto GameMonsterSnapshot::GetAttackMin() const -> float
    {
        return _attackMin;
    }

    auto GameMonsterSnapshot::GetAttackMax() const -> float
    {
        return _attackMax;
    }

    auto GameMonsterSnapshot::GetAttackRange() const -> float
    {
        return _attackRange;
    }

    auto GameMonsterSnapshot::GetAttackSpeed() const -> float
    {
        return _attackSpeed;
    }

    void GameMonsterSnapshot::SetPosition(const Eigen::Vector3d& position)
    {
        _position = position;
    }

    void GameMonsterSnapshot::SetYaw(float yaw)
    {
        _yaw = yaw;
    }

    void GameMonsterSnapshot::SetHp(float hp)
    {
        _hp = hp;
    }

    void GameMonsterSnapshot::SetMaxHp(float maxHp)
    {
        _maxHP = maxHp;
    }

    void GameMonsterSnapshot::SetAttackMin(float attackMin)
    {
        _attackMin = attackMin;
    }

    void GameMonsterSnapshot::SetAttackMax(float attackMax)
    {
        _attackMax = attackMax;
    }

    void GameMonsterSnapshot::SetAttackRange(float attackRange)
    {
        _attackRange = attackRange;
    }

    void GameMonsterSnapshot::SetAttackSpeed(float attackSpeed)
    {
        _attackSpeed = attackSpeed;
    }
}
