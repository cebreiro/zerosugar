#include "monster_data_provider.h"

namespace zerosugar::xr
{
    void MonsterDataProvider::Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath)
    {
        _bases = LoadJsonFromFile<data::Monster>(serviceLocator, basePath / "table" / "MonsterTable.json");
        _skills = LoadJsonFromFile<data::MonsterSkill>(serviceLocator, basePath / "table" / "MonsterSkillTable.json");
        _animations = LoadJsonFromFile<data::MonsterAnimation>(serviceLocator, basePath / "table" / "MonsterAnimation.json");

        for (const data::Monster& base : _bases)
        {
            [[maybe_unused]]
            const bool inserted = _monsters.try_emplace(base.id, &base).second;
            assert(inserted);
        }

        for (const data::MonsterSkill& skill : _skills)
        {
            const auto iter = _monsters.find(skill.monsterId);
            if (iter == _monsters.end())
            {
                assert(false);

                continue;
            }

            iter->second.AddSkill(skill.id, &skill);
        }

        for (const data::MonsterAnimation& animation : _animations)
        {
            const auto iter = _monsters.find(animation.monsterId);
            if (iter == _monsters.end())
            {
                assert(false);

                continue;
            }

            for (const data::MonsterAnimation::Value& value : animation.values)
            {
                iter->second.AddSkillAnimation(value.name, &value);
            }
        }
    }

    auto MonsterDataProvider::Find(int32_t mobId) const -> const MonsterData*
    {
        const auto iter = _monsters.find(mobId);

        return iter != _monsters.end() ? &iter->second : nullptr;

    }

    auto MonsterDataProvider::GetName() const -> std::string_view
    {
        return "monster_data_provider";
    }

    template <typename T>
    auto MonsterDataProvider::LoadJsonFromFile(ServiceLocator& serviceLocator, const std::filesystem::path& path) const -> std::vector<T>
    {
        std::vector<T> result;
        
        if (!exists(path))
        {
            ZEROSUGAR_LOG_ERROR(serviceLocator,
                fmt::format("[{}] fail to find directory. path: {}",
                    GetName(), path.generic_string()));

            return result;
        }

        try
        {
            std::ifstream ifstream(path, std::ios::in | std::ios::binary);
            if (!ifstream.is_open())
            {
                throw std::runtime_error("fail to open");
            }

            nlohmann::json json;
            ifstream >> json;

            from_json(json.at("list"), result);
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_ERROR(serviceLocator,
                fmt::format("[{}] fail to initialize map data. exception: {}, path: {}",
                    GetName(), e.what(), path.generic_string()));
        }

        return result;
    }
}
