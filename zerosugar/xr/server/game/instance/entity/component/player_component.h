#pragma once
#include "zerosugar/xr/server/game/instance/entity/component/game_component.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"

namespace zerosugar::xr::service
{
    struct DTOCharacter;
}

namespace zerosugar::xr
{
    class PlayerComponent : public GameComponent
    {
    public:
        explicit PlayerComponent(const service::DTOCharacter& dto);

        bool IsDodgeState(game_time_point_type current = GameExecutionParallel::GetBaseTimePoint()) const;

        auto GetCharacterId() const -> int64_t;
        auto GetName() const -> const std::string&;
        auto GetLevel() const -> int32_t;
        auto GetJob() const -> int32_t;
        auto GetFaceId() const -> int32_t;
        auto GetHairId() const -> int32_t;
        void SetRollDodgeStartTimePoint(game_time_point_type timePoint);

    private:
        int64_t _characterId = 0;
        std::string _name;
        int32_t _level = 0;
        int32_t _job = 0;
        int32_t _faceId = 0;
        int32_t _hairId = 0;

        game_time_point_type _rollDodgeStartTimePoint = game_time_point_type::min();
    };
}
