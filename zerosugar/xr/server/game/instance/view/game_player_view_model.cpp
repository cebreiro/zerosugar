#include "game_player_view_model.h"

#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/player_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"

namespace zerosugar::xr
{
    GamePlayerViewModel::GamePlayerViewModel(IGameController& controller)
        : _controller(controller)
    {
    }

    void GamePlayerViewModel::Initialize(const GameEntity& entity)
    {
        _id = entity.GetId();

        auto& playerComponent = entity.GetComponent<PlayerComponent>();
        auto& statComponent = entity.GetComponent<StatComponent>();

        _hp = 100;// statComponent.GetHP().As<float>();
        _maxHP = 100;// statComponent.GetMaxHP().As<float>();
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
        _stamina = statComponent.GetStamina().As<float>();
        _staminaMax = statComponent.GetStamina().As<float>();

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

    auto GamePlayerViewModel::GetController() const -> IGameController&
    {
        return _controller;
    }

    auto GamePlayerViewModel::GetId() const -> game_entity_id_type
    {
        return _id;
    }

    auto GamePlayerViewModel::GetPosition() const -> const Eigen::Vector3d&
    {
        return _position;
    }

    auto GamePlayerViewModel::GetYaw() const -> float
    {
        return _yaw;
    }

    auto GamePlayerViewModel::GetHp() const -> float
    {
        return _hp;
    }

    auto GamePlayerViewModel::GetMaxHp() const -> float
    {
        return _maxHP;
    }

    auto GamePlayerViewModel::GetAttackMin() const -> float
    {
        return _attackMin;
    }

    auto GamePlayerViewModel::GetAttackMax() const -> float
    {
        return _attackMax;
    }

    auto GamePlayerViewModel::GetAttackRange() const -> float
    {
        return _attackRange;
    }

    auto GamePlayerViewModel::GetAttackSpeed() const -> float
    {
        return _attackSpeed;
    }

    auto GamePlayerViewModel::GetSpeed() const -> float
    {
        return _speed;
    }

    auto GamePlayerViewModel::GetDefence() const -> float
    {
        return _defence;
    }

    auto GamePlayerViewModel::GetName() const -> const std::string&
    {
        return _name;
    }

    auto GamePlayerViewModel::GetLevel() const -> int32_t
    {
        return _level;
    }

    auto GamePlayerViewModel::GetGender() const -> int32_t
    {
        return _gender;
    }

    auto GamePlayerViewModel::GetFace() const -> int32_t
    {
        return _face;
    }

    auto GamePlayerViewModel::GetHair() const -> int32_t
    {
        return _hair;
    }

    auto GamePlayerViewModel::GetStr() const -> int32_t
    {
        return _str;
    }

    auto GamePlayerViewModel::GetDex() const -> int32_t
    {
        return _dex;
    }

    auto GamePlayerViewModel::GetIntell() const -> int32_t
    {
        return _intell;
    }

    auto GamePlayerViewModel::GetStamina() const -> float
    {
        return _stamina;
    }

    auto GamePlayerViewModel::GetStaminaMax() const -> float
    {
        return _staminaMax;
    }

    auto GamePlayerViewModel::GetEquipment() const -> const std::array<std::optional<InventoryItem>, static_cast<int32_t>(data::EquipPosition::Count)>&
    {
        return _equipItems;
    }

    void GamePlayerViewModel::SetPosition(const Eigen::Vector3d& position)
    {
        _position = position;
    }

    void GamePlayerViewModel::SetYaw(float yaw)
    {
        _yaw = yaw;
    }

    void GamePlayerViewModel::SetHp(float hp)
    {
        _hp = hp;
    }

    void GamePlayerViewModel::SetMaxHp(float maxHp)
    {
        _maxHP = maxHp;
    }

    void GamePlayerViewModel::SetAttackMin(float attackMin)
    {
        _attackMin = attackMin;
    }

    void GamePlayerViewModel::SetAttackMax(float attackMax)
    {
        _attackMax = attackMax;
    }

    void GamePlayerViewModel::SetAttackRange(float attackRange)
    {
        _attackRange = attackRange;
    }

    void GamePlayerViewModel::SetAttackSpeed(float attackSpeed)
    {
        _attackSpeed = attackSpeed;
    }

    void GamePlayerViewModel::SetSpeed(float speed)
    {
        _speed = speed;
    }

    void GamePlayerViewModel::SetDefence(float defence)
    {
        _defence = defence;
    }

    void GamePlayerViewModel::SetName(const std::string& name)
    {
        _name = name;
    }

    void GamePlayerViewModel::SetLevel(int32_t level)
    {
        _level = level;
    }

    void GamePlayerViewModel::SetGender(int32_t gender)
    {
        _gender = gender;
    }

    void GamePlayerViewModel::SetFace(int32_t face)
    {
        _face = face;
    }

    void GamePlayerViewModel::SetHair(int32_t hair)
    {
        _hair = hair;
    }

    void GamePlayerViewModel::SetStr(int32_t str)
    {
        _str = str;
    }

    void GamePlayerViewModel::SetDex(int32_t dex)
    {
        _dex = dex;
    }

    void GamePlayerViewModel::SetIntell(int32_t intell)
    {
        _intell = intell;
    }

    void GamePlayerViewModel::SetStamina(float stamina)
    {
        _stamina = stamina;
    }

    void GamePlayerViewModel::SetStaminaMax(float staminaMax)
    {
        _staminaMax = staminaMax;
    }

    void GamePlayerViewModel::SetEquipment(data::EquipPosition position, std::optional<InventoryItem> item)
    {
        _equipItems[static_cast<int32_t>(position)] = item;
    }
}
