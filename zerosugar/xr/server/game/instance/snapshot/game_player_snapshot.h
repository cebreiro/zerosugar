#pragma once
#include "zerosugar/xr/server/game/instance/entity/component/inventory_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameEntity;
    class IGameController;
}

namespace zerosugar::xr
{
    class GamePlayerSnapshot
    {
    public:
        GamePlayerSnapshot() = delete;
        explicit GamePlayerSnapshot(IGameController& controller);

        void Initialize(const GameEntity& entity);

        auto GetController() const -> IGameController&;
        auto GetId() const -> game_entity_id_type;
        auto GetPosition() const -> const Eigen::Vector3d&;
        auto GetYaw() const -> float;
        auto GetHp() const -> float;
        auto GetMaxHp() const -> float;
        auto GetAttackMin() const -> float;
        auto GetAttackMax() const -> float;
        auto GetAttackRange() const -> float;
        auto GetAttackSpeed() const -> float;
        auto GetSpeed() const -> float;
        auto GetDefence() const -> float;
        auto GetName() const -> const std::string&;
        auto GetLevel() const -> int32_t;
        auto GetGender() const -> int32_t;
        auto GetFace() const -> int32_t;
        auto GetHair() const -> int32_t;
        auto GetStr() const -> int32_t;
        auto GetDex() const -> int32_t;
        auto GetIntell() const -> int32_t;
        auto GetStamina() const -> float;
        auto GetStaminaMax() const -> float;
        auto GetEquipment(data::EquipPosition position) const -> const InventoryItem*;
        auto GetEquipments() const -> const std::array<std::optional<InventoryItem>, static_cast<int32_t>(data::EquipPosition::Count)>&;

        void SetPosition(const Eigen::Vector3d& position);
        void SetYaw(float yaw);
        void SetHp(float hp);
        void SetMaxHp(float maxHp);
        void SetAttackMin(float attackMin);
        void SetAttackMax(float attackMax);
        void SetAttackRange(float attackRange);
        void SetAttackSpeed(float attackSpeed);
        void SetSpeed(float speed);
        void SetDefence(float defence);
        void SetName(const std::string& name);
        void SetLevel(int32_t level);
        void SetGender(int32_t gender);
        void SetFace(int32_t face);
        void SetHair(int32_t hair);
        void SetStr(int32_t str);
        void SetDex(int32_t dex);
        void SetIntell(int32_t intell);
        void SetStamina(float stamina);
        void SetStaminaMax(float staminaMax);
        void SetEquipment(data::EquipPosition position, std::optional<InventoryItem> item);

    private:
        IGameController& _controller;

        game_entity_id_type _id;
        Eigen::Vector3d _position;
        float _yaw = 0.f;
        float _hp = 0.f;
        float _maxHP = 0.f;
        float _attackMin = 0.f;
        float _attackMax = 0.f;
        float _attackRange = 0.f;
        float _attackSpeed = 0.f;
        float _speed = 0.f;
        float _defence = 0.f;
        std::string _name ;
        int32_t _level = 0;
        int32_t _gender = 0;
        int32_t _face = 0;
        int32_t _hair = 0;
        int32_t _str = 0;
        int32_t _dex = 0;
        int32_t _intell = 0;
        float _stamina = 0.f;
        float _staminaMax = 0.f;

        std::array<std::optional<InventoryItem>, static_cast<int32_t>(data::EquipPosition::Count)> _equipItems;
    };
}
