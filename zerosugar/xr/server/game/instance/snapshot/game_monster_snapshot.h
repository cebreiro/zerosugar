#pragma once
#include <Eigen/Dense>

#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameEntity;
    class IGameController;
}

namespace zerosugar::xr
{
    class GameMonsterSnapshot
    {
    public:
        GameMonsterSnapshot(IGameController& controller, game_entity_id_type id, int32_t dataId);

        void Initialize(const GameEntity& entity);

        auto GetController() const -> IGameController&;
        auto GetId() const -> game_entity_id_type;
        auto GetDataId() const -> int32_t;
        auto GetPosition() const -> const Eigen::Vector3d&;
        auto GetYaw() const -> float;
        auto GetHp() const -> float;
        auto GetMaxHp() const -> float;
        auto GetAttackMin() const -> float;
        auto GetAttackMax() const -> float;
        auto GetAttackRange() const -> float;
        auto GetAttackSpeed() const -> float;

        void SetPosition(const Eigen::Vector3d& position);
        void SetYaw(float yaw);
        void SetHp(float hp);
        void SetMaxHp(float maxHp);
        void SetAttackMin(float attackMin);
        void SetAttackMax(float attackMax);
        void SetAttackRange(float attackRange);
        void SetAttackSpeed(float attackSpeed);

    private:
        IGameController& _controller;

        game_entity_id_type _id;
        int32_t _dataId = 0;
        Eigen::Vector3d _position;
        float _yaw = 0.f;
        float _hp = 0.f;
        float _maxHP = 0.f;
        float _attackMin = 0.f;
        float _attackMax = 0.f;
        float _attackRange = 0.f;
        float _attackSpeed = 0.f;
    };
}
