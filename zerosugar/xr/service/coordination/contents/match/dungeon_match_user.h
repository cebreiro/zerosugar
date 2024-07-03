#pragma once
#include "zerosugar/xr/service/coordination/node/node_id.h"

namespace zerosugar::xr
{
    ENUM_CLASS(DungeonMatchUserState, int32_t,
        (Waiting)
        (Grouped)
    )

    class DungeonMatchUser
    {
    public:
        DungeonMatchUser() = default;
        explicit DungeonMatchUser(coordination::game_user_id_type id, int32_t dungeonId,
            std::chrono::system_clock::time_point creationTimePoint);

        auto GetId() const -> coordination::game_user_id_type;
        auto GetCreationTimePoint() const ->std::chrono::system_clock::time_point;
        auto GetDungeonId() const -> int32_t;
        auto GetState() const -> DungeonMatchUserState;
        auto GetGroupId() const -> std::optional<int64_t>;

        void SetState(DungeonMatchUserState state);
        void SetGroupId(std::optional<int64_t> id);

    private:
        coordination::game_user_id_type _id;
        int32_t _dungeonId = 0;
        std::chrono::system_clock::time_point _creationTimePoint;

        DungeonMatchUserState _state = DungeonMatchUserState::Waiting;
        std::optional<int64_t>  _groupId = std::nullopt;
    };
}
