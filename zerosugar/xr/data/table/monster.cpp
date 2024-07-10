#include "monster.h"

#include <boost/lexical_cast.hpp>

namespace zerosugar::xr::data
{
    void from_json(const nlohmann::json& json, Monster& monster)
    {
        monster.id = boost::lexical_cast<int32_t>(json.at("Name").get<std::string>());

        json.at("MonsterMaxHP").get_to(monster.hpMax);
        json.at("MonsterAttackMin").get_to(monster.attackMin);
        json.at("MonsterAttackMax").get_to(monster.attackMax);
        json.at("MonsterAttackRange").get_to(monster.attackRange);
        json.at("MonsterAttackSpeed").get_to(monster.attackSpeed);
        json.at("MonsterSpeed").get_to(monster.speed);
        json.at("BehaviorTree").get_to(monster.behaviorTree);
    }

    void from_json(const nlohmann::json& json, MonsterSkill& skill)
    {
        skill.id = boost::lexical_cast<int32_t>(json.at("Name").get<std::string>());

        json.at("SkillName").get_to(skill.name);
        json.at("MonsterId").get_to(skill.monsterId);
        json.at("CoolDown").get_to(skill.cooldown);
        json.at("IsKnockBack").get_to(skill.knockBack);
    }

    void from_json(const nlohmann::json& json, MonsterAnimation& animation)
    {
        json.at("MonsterId").get_to(animation.monsterId);

        if (const auto iter = json.find("Animation"); iter != json.end())
        {
            for (const nlohmann::json& element : *iter)
            {
                MonsterAnimation::Value& value = animation.values.emplace_back();

                element.at("Name").get_to(value.name);
                element.at("Duration").get_to(value.duration);

                if (const auto it = element.find("Movement"); it != element.end())
                {
                    it->get_to(value.forwardMovement);
                }

                if (const auto iter2 = element.find("Index"); iter2 != element.end())
                {
                    iter2->get_to(value.index);
                }

                if (const auto iter2 = element.find("Attack"); iter2 != element.end())
                {
                    for (const nlohmann::json& e : *iter2)
                    {
                        MonsterAnimation::Attack& attack = value.attacks.emplace_back();

                        e.at("ApplyTimePoint").get_to(attack.applyTimePoint);

                        const auto& collision = e.at("Collision");

                        collision.at("MinX").get_to(attack.minX);
                        collision.at("MaxX").get_to(attack.maxX);
                        collision.at("MinY").get_to(attack.minY);
                        collision.at("MaxY").get_to(attack.maxY);
                    }
                }
            }
        }
    }
}

namespace zerosugar::xr
{
    MonsterData::MonsterData(PtrNotNull<const data::Monster> base)
        : _base(base)
    {
    }

    void MonsterData::AddSkill(int32_t skillId, PtrNotNull<const data::MonsterSkill> data)
    {
        [[maybe_unused]]
        const bool inserted = _skills.try_emplace(skillId, data).second;
        assert(inserted);
    }

    void MonsterData::AddSkillAnimation(const std::string& name, PtrNotNull<const data::MonsterAnimation::Value> data)
    {
        [[maybe_unused]]
        const bool inserted = _animations.try_emplace(name, data).second;
        assert(inserted);
    }

    auto MonsterData::FindSkill(int32_t skillId) const -> const data::MonsterSkill*
    {
        const auto iter = _skills.find(skillId);

        return iter != _skills.end() ? iter->second : nullptr;
    }

    auto MonsterData::FindAnimation(const std::string& name) const -> const data::MonsterAnimation::Value*
    {
        const auto iter = _animations.find(name);

        return iter != _animations.end() ? iter->second : nullptr;
    }

    auto MonsterData::GetBase() const -> const data::Monster&
    {
        return *_base;
    }
}
