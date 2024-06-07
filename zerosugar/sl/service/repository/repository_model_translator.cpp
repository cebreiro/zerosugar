#include "repository_model_translator.h"

namespace zerosugar::sl
{
    auto RepositoryModelTranslator::ToServiceModel(const DatabaseModelRef& param)
        -> service::Character
    {
        service::Character result;
        result.slot = param.character.slot;
        result.name = param.character.name;
        result.hairColor = param.character.hair_color;
        result.hair = param.character.hair;
        result.skinColor = param.character.skin_color;
        result.face = param.character.face;
        result.arms = param.character.arms;
        result.running = param.character.running;
        result.gold = param.character.gold;
        result.inventorySize = param.character.inventory_page;
        result.zone = param.character.zone;
        result.stage = param.character.stage;
        result.x = param.character.x;
        result.y = param.character.y;

        result.stat = ToServiceStat(param.stat);
        result.job = ToServiceJob(param.job);
        result.item = param.items | std::views::transform(ToServiceItem) | std::ranges::to<std::vector>();
        result.skill = param.skills | std::views::transform(ToServiceSkill) | std::ranges::to<std::vector>();

        return result;
    }

    auto RepositoryModelTranslator::ToDatabaseModel(const service::Character& param) -> DatabaseModel
    {
        DatabaseModel result;
        result.character.slot = static_cast<int8_t>(param.slot);
        result.character.name = param.name;
        result.character.hair_color = param.hairColor;
        result.character.hair = param.hair;
        result.character.skin_color = param.skinColor;
        result.character.face = param.face;
        result.character.arms = param.arms;
        result.character.running = param.running;
        result.character.gold = param.gold;
        result.character.inventory_page = param.inventorySize;
        result.character.zone = param.zone;
        result.character.stage = param.stage;
        result.character.x = param.x;
        result.character.y = param.y;

        result.stat = ToDatabaseStat(param.stat);
        result.job = ToDatabaseJob(param.job);
        result.items = param.item | std::views::transform(ToDatabaseItem) | std::ranges::to<std::vector>();
        result.skills = param.skill | std::views::transform(ToDatabaseSkill) | std::ranges::to<std::vector>();

        return result;
    }

    auto RepositoryModelTranslator::ToServiceStat(const db::CharacterStat& stat) -> service::CharacterStat
    {
        service::CharacterStat result;
        result.gender = stat.gender;
        result.hp = stat.hp;
        result.mp = stat.mp;
        result.level = stat.chr_lv;
        result.exp = stat.chr_exp;
        result.str = stat.str;
        result.dex = stat.dex;
        result.accr = stat.accr;
        result.health = stat.health;
        result.intell = stat.intell;
        result.wis = stat.wis;
        result.will = stat.will;
        result.statPoint = stat.stat_point;
        result.elementalWater = stat.water;
        result.elementalFire = stat.fire;
        result.elementalLightning = stat.lightning;

        return result;
    }

    auto RepositoryModelTranslator::ToDatabaseStat(const service::CharacterStat& stat) -> db::CharacterStat
    {
        db::CharacterStat result;
        result.gender = stat.gender;
        result.hp = stat.hp;
        result.mp = stat.mp;
        result.chr_lv = stat.level;
        result.chr_exp = stat.exp;
        result.str = stat.str;
        result.dex = stat.dex;
        result.accr = stat.accr;
        result.health = stat.health;
        result.intell = stat.intell;
        result.wis = stat.wis;
        result.will = stat.will;
        result.stat_point = stat.statPoint;
        result.water = stat.elementalWater;
        result.fire = stat.elementalFire;
        result.lightning = stat.elementalLightning;

        return result;
    }

    auto RepositoryModelTranslator::ToServiceJob(const db::CharacterJob& job) -> std::vector<service::Job>
    {
        std::vector<service::Job> result{
            service::Job{
                .grade = service::JobGradeType::JobGradeNovice,
                .dataId = job.job1,
                .level = job.job1_level,
                .exp = job.job1_exp,
                .sp = job.job1_sp,
            },
            service::Job{
                .grade = service::JobGradeType::JobGradeAdvanced1,
                .dataId = job.job2,
                .level = job.job2_level,
                .exp = job.job2_exp,
                .sp = job.job2_sp,
            },
            service::Job{
                .grade = service::JobGradeType::JobGradeAdvanced2,
                .dataId = job.job3,
                .level = job.job3_level,
                .exp = job.job3_exp,
                .sp = job.job3_sp,
            },
            service::Job{
                .grade = service::JobGradeType::JobGradeSub,
                .dataId = job.job_sub,
                .level = job.job_sub_level,
                .exp = job.job_sub_exp,
                .sp = job.job_sub_sp,
            }
        };
        std::erase_if(result, [](const service::Job& job)
            {
                return job.dataId <= 0;
            });

        return result;
    }

    auto RepositoryModelTranslator::ToDatabaseJob(const std::vector<service::Job>& jobs) -> db::CharacterJob
    {
        db::CharacterJob result = {};

        for (const service::Job& job : jobs)
        {
            switch (job.grade)
            {
            case service::JobGradeType::JobGradeNovice:
            {
                result.job1 = job.dataId;
                result.job1_level = job.level;
                result.job1_exp = job.exp;
                result.job1_sp = job.sp;
            }
            break;
            case service::JobGradeType::JobGradeAdvanced1:
            {
                result.job2 = job.dataId;
                result.job2_level = job.level;
                result.job2_exp = job.exp;
                result.job2_sp = job.sp;
            }
            break;
            case service::JobGradeType::JobGradeAdvanced2:
            {
                result.job3 = job.dataId;
                result.job3_level = job.level;
                result.job3_exp = job.exp;
                result.job3_sp = job.sp;
            }
            break;
            case service::JobGradeType::JobGradeSub:
            {
                result.job_sub = job.dataId;
                result.job_sub_level = job.level;
                result.job_sub_exp = job.exp;
                result.job_sub_sp = job.sp;
            }
            break;
            }
        }

        return result;
    }

    auto RepositoryModelTranslator::ToServiceItem(const db::Item& item) -> service::Item
    {
        return service::Item{
            .uniqueId = item.id,
            .ownerId = item.owner_id,
            .dataId = item.data_id,
            .quantity = item.quantity,
            .position = service::SlotPosition{
                .type = static_cast<service::SlotPositionType>(item.pos_type),
                .value1 = item.pos_value1,
                .value2 = item.pos_value2,
                .value3 = item.pos_value3,
            },
        };
    }

    auto RepositoryModelTranslator::ToDatabaseItem(const service::Item& item) -> db::Item
    {
        return db::Item{
            .id = item.uniqueId,
            .owner_id = item.ownerId,
            .data_id = item.dataId,
            .quantity = item.quantity,
            .pos_type = static_cast<int8_t>(item.position.type),
            .pos_value1 = static_cast<int8_t>(item.position.value1),
            .pos_value2 = static_cast<int8_t>(item.position.value2),
            .pos_value3 = static_cast<int8_t>(item.position.value3),
        };
    }

    auto RepositoryModelTranslator::ToServiceSkill(const db::Skill& skill) -> service::Skill
    {
        return service::Skill{
            .uniqueId = skill.id,
            .ownerId = skill.owner_id,
            .jobId = skill.job_id,
            .skillId = skill.skill_id,
            .level = skill.level,
            .cooldown = skill.cooldown,
            .position = service::SlotPosition{
                .type = static_cast<service::SlotPositionType>(skill.pos_type),
                .value1 = skill.pos_value1,
                .value2 = skill.pos_value2,
                .value3 = skill.pos_value3,
            },
        };
    }

    auto RepositoryModelTranslator::ToDatabaseSkill(const service::Skill& skill) -> db::Skill
    {
        return db::Skill{
            .id = skill.uniqueId,
            .owner_id = skill.ownerId,
            .job_id = skill.jobId,
            .skill_id = skill.skillId,
            .level = skill.level,
            .cooldown = skill.cooldown,
            .pos_type = static_cast<int8_t>(skill.position.type),
            .pos_value1 = static_cast<int8_t>(skill.position.value1),
            .pos_value2 = static_cast<int8_t>(skill.position.value2),
            .pos_value3 = static_cast<int8_t>(skill.position.value3),
        };
    }
}
