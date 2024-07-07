#include "monster_data_provider.h"

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
}

namespace zerosugar::xr
{
    void MonsterDataProvider::Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath)
    {
        const auto path = basePath / "table" / "MonsterTable.json";
        if (!exists(path))
        {
            ZEROSUGAR_LOG_ERROR(serviceLocator,
                std::format("[{}] fail to find navigation directory. path: {}",
                    GetName(), path.generic_string()));

            return;
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

            std::vector<data::Monster> monsters;
            from_json(json.at("list"), monsters);

            for (const data::Monster& monster : monsters)
            {
                if (!_monsters.try_emplace(monster.id, monster).second)
                {
                    ZEROSUGAR_LOG_ERROR(serviceLocator,
                        std::format("[{}] fail to insert monster data. duplicated id: {}, path: {}",
                            GetName(), monster.id, path.generic_string()));
                }
            }
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_ERROR(serviceLocator,
                std::format("[{}] fail to initialize map data. exception: {}, path: {}",
                    GetName(), e.what(), path.generic_string()));
        }
    }

    auto MonsterDataProvider::Find(int32_t mobId) const -> const data::Monster*
    {
        const auto iter = _monsters.find(mobId);

        return iter != _monsters.end() ? &iter->second : nullptr;

    }

    auto MonsterDataProvider::GetName() const -> std::string_view
    {
        return "monster_data_provider";
    }
}
