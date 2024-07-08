#include "game_monster_snapshot.h"

#include "zerosugar/xr/data/table/monster.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"

namespace zerosugar::xr
{
    GameMonsterSnapshot::GameMonsterSnapshot(IGameController& controller, game_entity_id_type id, const MonsterData& data)
        : _controller(controller)
        , _id(id)
        , _data(data)
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

    auto GameMonsterSnapshot::GetData() const -> const MonsterData&
    {
        return _data;
    }

    auto GameMonsterSnapshot::GetDataId() const -> int32_t
    {
        return _data.GetBase().id;
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
        return _data.GetBase().attackMin;
    }

    auto GameMonsterSnapshot::GetAttackMax() const -> float
    {
        return _data.GetBase().attackMax;
    }

    auto GameMonsterSnapshot::GetAttackRange() const -> float
    {
        return _data.GetBase().attackRange;
    }

    auto GameMonsterSnapshot::GetAttackSpeed() const -> float
    {
        return _data.GetBase().attackSpeed;
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
}
