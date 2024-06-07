#pragma once
#include "zerosugar/sl/service/generated/shared_generated.h"
#include "zerosugar/sl/database/model/character.h"
#include "zerosugar/sl/database/model/item.h"
#include "zerosugar/sl/database/model/skill.h"

namespace zerosugar::sl
{
    struct DatabaseModel
    {
        db::Character character;
        db::CharacterStat stat;
        db::CharacterJob job;
        std::vector<db::Item> items;
        std::vector<db::Skill> skills;
    };

    struct DatabaseModelRef
    {
        const db::Character& character;
        const db::CharacterStat& stat;
        const db::CharacterJob& job;
        const std::vector<db::Item>& items;
        const std::vector<db::Skill>& skills;
    };

    class RepositoryModelTranslator
    {
    public:
        RepositoryModelTranslator() = delete;
        static auto ToServiceModel(const DatabaseModelRef& param) -> service::Character;
        static auto ToDatabaseModel(const service::Character& param) -> DatabaseModel;

    private:
        static auto ToServiceStat(const db::CharacterStat& stat) -> service::CharacterStat;
        static auto ToDatabaseStat(const service::CharacterStat& stat) -> db::CharacterStat;

        static auto ToServiceJob(const db::CharacterJob& job) -> std::vector<service::Job>;
        static auto ToDatabaseJob(const std::vector<service::Job>& jobs) -> db::CharacterJob;

        static auto ToServiceItem(const db::Item& item) -> service::Item;
        static auto ToDatabaseItem(const service::Item& item) -> db::Item;

        static auto ToServiceSkill(const db::Skill& skill) -> service::Skill;
        static auto ToDatabaseSkill(const service::Skill& skill) -> db::Skill;
    };
}
