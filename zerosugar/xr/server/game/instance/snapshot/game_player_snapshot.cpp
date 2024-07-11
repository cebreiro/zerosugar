#include "game_player_snapshot.h"

#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/player_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"

namespace zerosugar::xr
{
    GamePlayerSnapshot::GamePlayerSnapshot(IGameController& controller)
        : _controller(controller)
    {
    }

    void GamePlayerSnapshot::Initialize(const GameEntity& entity)
    {
        _id = entity.GetId();

        auto& playerComponent = entity.GetComponent<PlayerComponent>();
        auto& statComponent = entity.GetComponent<StatComponent>();

        auto now = game_clock_type::now();

        _hp = statComponent.GetHP(now).As<float>();
        _maxHP = statComponent.GetMaxHP().As<float>();
        _attackMin = statComponent.Get(StatType::Attack).As<float>();
        _attackMax = statComponent.Get(StatType::Attack).As<float>();
        _speed = 10.f;
        _name = playerComponent.GetName();
        _level = playerComponent.GetLevel();
        _gender = 0;
        _face = playerComponent.GetFaceId();
        _hair = playerComponent.GetHairId();
        _str = statComponent.Get(StatType::Str).As<int32_t>();
        _dex = statComponent.Get(StatType::Dex).As<int32_t>();
        _intell = statComponent.Get(StatType::Intell).As<int32_t>();
        _stamina = statComponent.GetStamina(now).As<float>();
        _staminaMax = statComponent.GetMaxStamina().As<float>();

        _position = entity.GetComponent<MovementComponent>().GetPosition();

        {
            auto& inventoryComponent = entity.GetComponent<InventoryComponent>();

            const auto& equipItems = inventoryComponent.GetEquipments();

            for (int32_t i = 0; i < static_cast<int32_t>(std::ssize(equipItems)); ++i)
            {
                const InventoryItem* item = equipItems[i];
                if (!item)
                {
                    continue;
                }

                _equipItems[i] = *item;
            }
        }
    }

    auto GamePlayerSnapshot::GetController() const -> IGameController&
    {
        return _controller;
    }

    auto GamePlayerSnapshot::GetId() const -> game_entity_id_type
    {
        return _id;
    }

    auto GamePlayerSnapshot::GetPosition() const -> const Eigen::Vector3d&
    {
        return _position;
    }

    auto GamePlayerSnapshot::GetYaw() const -> float
    {
        return _yaw;
    }

    auto GamePlayerSnapshot::GetHp() const -> float
    {
        return _hp;
    }

    auto GamePlayerSnapshot::GetMaxHp() const -> float
    {
        return _maxHP;
    }

    auto GamePlayerSnapshot::GetAttackMin() const -> float
    {
        return _attackMin;
    }

    auto GamePlayerSnapshot::GetAttackMax() const -> float
    {
        return _attackMax;
    }

    auto GamePlayerSnapshot::GetAttackRange() const -> float
    {
        return _attackRange;
    }

    auto GamePlayerSnapshot::GetAttackSpeed() const -> float
    {
        return _attackSpeed;
    }

    auto GamePlayerSnapshot::GetSpeed() const -> float
    {
        return _speed;
    }

    auto GamePlayerSnapshot::GetDefence() const -> float
    {
        return _defence;
    }

    auto GamePlayerSnapshot::GetName() const -> const std::string&
    {
        return _name;
    }

    auto GamePlayerSnapshot::GetLevel() const -> int32_t
    {
        return _level;
    }

    auto GamePlayerSnapshot::GetGender() const -> int32_t
    {
        return _gender;
    }

    auto GamePlayerSnapshot::GetFace() const -> int32_t
    {
        return _face;
    }

    auto GamePlayerSnapshot::GetHair() const -> int32_t
    {
        return _hair;
    }

    auto GamePlayerSnapshot::GetStr() const -> int32_t
    {
        return _str;
    }

    auto GamePlayerSnapshot::GetDex() const -> int32_t
    {
        return _dex;
    }

    auto GamePlayerSnapshot::GetIntell() const -> int32_t
    {
        return _intell;
    }

    auto GamePlayerSnapshot::GetStamina() const -> float
    {
        return _stamina;
    }

    auto GamePlayerSnapshot::GetStaminaMax() const -> float
    {
        return _staminaMax;
    }

    auto GamePlayerSnapshot::GetEquipment(data::EquipPosition position) const -> const InventoryItem*
    {
        assert(IsValid(position));

        const std::optional<InventoryItem>& item = _equipItems[static_cast<int32_t>(position)];

        return item.has_value() ? &item.value() : nullptr;
    }

    auto GamePlayerSnapshot::GetEquipments() const -> const std::array<std::optional<InventoryItem>, static_cast<int32_t>(data::EquipPosition::Count)>&
    {
        return _equipItems;
    }

    void GamePlayerSnapshot::SetPosition(const Eigen::Vector3d& position)
    {
        _position = position;
    }

    void GamePlayerSnapshot::SetYaw(float yaw)
    {
        _yaw = yaw;
    }

    void GamePlayerSnapshot::SetHp(float hp)
    {
        _hp = hp;
    }

    void GamePlayerSnapshot::SetMaxHp(float maxHp)
    {
        _maxHP = maxHp;
    }

    void GamePlayerSnapshot::SetAttackMin(float attackMin)
    {
        _attackMin = attackMin;
    }

    void GamePlayerSnapshot::SetAttackMax(float attackMax)
    {
        _attackMax = attackMax;
    }

    void GamePlayerSnapshot::SetAttackRange(float attackRange)
    {
        _attackRange = attackRange;
    }

    void GamePlayerSnapshot::SetAttackSpeed(float attackSpeed)
    {
        _attackSpeed = attackSpeed;
    }

    void GamePlayerSnapshot::SetSpeed(float speed)
    {
        _speed = speed;
    }

    void GamePlayerSnapshot::SetDefence(float defence)
    {
        _defence = defence;
    }

    void GamePlayerSnapshot::SetName(const std::string& name)
    {
        _name = name;
    }

    void GamePlayerSnapshot::SetLevel(int32_t level)
    {
        _level = level;
    }

    void GamePlayerSnapshot::SetGender(int32_t gender)
    {
        _gender = gender;
    }

    void GamePlayerSnapshot::SetFace(int32_t face)
    {
        _face = face;
    }

    void GamePlayerSnapshot::SetHair(int32_t hair)
    {
        _hair = hair;
    }

    void GamePlayerSnapshot::SetStr(int32_t str)
    {
        _str = str;
    }

    void GamePlayerSnapshot::SetDex(int32_t dex)
    {
        _dex = dex;
    }

    void GamePlayerSnapshot::SetIntell(int32_t intell)
    {
        _intell = intell;
    }

    void GamePlayerSnapshot::SetStamina(float stamina)
    {
        _stamina = stamina;
    }

    void GamePlayerSnapshot::SetStaminaMax(float staminaMax)
    {
        _staminaMax = staminaMax;
    }

    void GamePlayerSnapshot::SetEquipment(data::EquipPosition position, std::optional<InventoryItem> item)
    {
        _equipItems[static_cast<int32_t>(position)] = item;
    }
}
