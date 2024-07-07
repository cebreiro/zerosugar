#pragma once

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
}

namespace zerosugar::xr
{
    class MonsterDataProvider
    {
    public:
        void Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath);

        auto Find(int32_t mobId) const -> const data::Monster*;

        auto GetName() const -> std::string_view;

    private:
        std::unordered_map<int32_t, data::Monster> _monsters;
    };
}
