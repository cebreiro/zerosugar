#include "dungeon_match_user.h"

namespace zerosugar::xr
{
    DungeonMatchUser::DungeonMatchUser(coordination::game_user_id_type id, int32_t dungeonId, std::chrono::system_clock::time_point creationTimePoint)
        : _id(id)
        , _dungeonId(dungeonId)
        , _creationTimePoint(creationTimePoint)
    {
    }

    auto DungeonMatchUser::GetId() const -> coordination::game_user_id_type
    {
        return _id;
    }

    auto DungeonMatchUser::GetCreationTimePoint() const -> std::chrono::system_clock::time_point
    {
        return _creationTimePoint;
    }

    auto DungeonMatchUser::GetDungeonId() const -> int32_t
    {
        return _dungeonId;
    }

    auto DungeonMatchUser::GetState() const -> DungeonMatchUserState
    {
        return _state;
    }

    auto DungeonMatchUser::GetGroupId() const -> std::optional<int64_t>
    {
        return _groupId;
    }

    void DungeonMatchUser::SetState(DungeonMatchUserState state)
    {
        _state = state;
    }

    void DungeonMatchUser::SetGroupId(std::optional<int64_t> id)
    {
        _groupId = id;
    }
}
