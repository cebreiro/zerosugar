#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

namespace zerosugar::xr::data
{
    struct Monster
    {
        int32_t id = 0;
        int32_t hpMax = 0;
        float attackMin = 0.f;
        float attackMax = 0.f;
        float attackRange = 0.f;
        float attackSpeed = 0.f;
        float speed = 0.f;
        std::string behaviorTree;

        friend void from_json(const nlohmann::json& json, Monster& monster);
    };

    struct MonsterSkill
    {
        int32_t id = 0;
        std::string name;
        int32_t monsterId = 0;
        int32_t cooldown = 0;

        bool knockBack = false;

        friend void from_json(const nlohmann::json& json, MonsterSkill& skill);
    };

    struct MonsterAnimation
    {
        int32_t monsterId = 0;

        struct Value
        {
            std::string name;
            double duration;
            std::vector<double> attackPoints;
        };

        std::vector<Value> values;

        friend void from_json(const nlohmann::json& json, MonsterAnimation& animation);
    };
}

namespace zerosugar::xr
{
    class MonsterData
    {
    public:
        MonsterData() = default;

        explicit MonsterData(PtrNotNull<const data::Monster> base);

        void AddSkill(int32_t skillId, PtrNotNull<const data::MonsterSkill> data);
        void AddSkillAnimation(const std::string& name, PtrNotNull<const data::MonsterAnimation::Value> data);

        auto FindSkill(int32_t skillId) const -> const data::MonsterSkill*;
        auto FindAnimation(const std::string& name) const -> const data::MonsterAnimation::Value*;

        auto GetBase() const -> const data::Monster&;
        inline auto GetSkills() const;

    private:
        PtrNotNull<const data::Monster> _base = nullptr;

        boost::unordered::unordered_flat_map<int32_t, PtrNotNull<const data::MonsterSkill>> _skills;
        std::unordered_map<std::string, PtrNotNull<const data::MonsterAnimation::Value>> _animations;
    };

    auto MonsterData::GetSkills() const
    {
        return _skills
            | std::views::values
            | std::views::transform([](PtrNotNull<const data::MonsterSkill> ptr) -> const data::MonsterSkill&
                {
                    return *ptr;
                });
    }
}
