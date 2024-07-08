#pragma once
#include "zerosugar/xr/data/table/monster.h"

namespace zerosugar::xr
{
    class MonsterDataProvider
    {
    public:
        void Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath);

        auto Find(int32_t mobId) const -> const MonsterData*;

        auto GetName() const -> std::string_view;

    private:
        template <typename T>
        auto LoadJsonFromFile(ServiceLocator& serviceLocator, const std::filesystem::path& path) const -> std::vector<T>;

    private:
        std::vector<data::Monster> _bases;
        std::vector<data::MonsterSkill> _skills;
        std::vector<data::MonsterAnimation> _animations;

        std::unordered_map<int32_t, MonsterData> _monsters;
    };
}
