#pragma once
#include "zerosugar/xr/server/game/instance/ai/aggro/aggro.h"

namespace zerosugar::xr
{
    class IPacket;
    class AIController;
    class GamePlayerSnapshot;
}

namespace zerosugar::xr::ai
{
    class AggroContainer
    {
    public:
        static constexpr int64_t max_aggro_manage_count = 16;

    public:
        explicit AggroContainer(AIController& aiController);

        void Update();

        bool Empty() const;

        void Add(game_entity_id_type id, int64_t value);
        void Remove(game_entity_id_type id);

        auto GetPrimaryTarget() const -> GamePlayerSnapshot*;
        auto SelectPrimaryTarget() -> GamePlayerSnapshot*;

    private:
        static bool CompareAggroValue(const Aggro& lhs, const Aggro& rhs);

    private:
        AIController& _aiController;

        bool _dirty = false;
        std::vector<Aggro> _descSortedArray;
    };
}
