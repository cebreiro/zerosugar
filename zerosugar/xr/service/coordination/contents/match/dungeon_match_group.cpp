#include "dungeon_match_group.h"

namespace zerosugar::xr
{
    DungeonMatchGroup::DungeonMatchGroup(int64_t id, int32_t dungeonId, std::chrono::system_clock::time_point creationTimePoint)
        : _id(id)
        , _dungeonId(dungeonId)
        , _creationTimePoint(creationTimePoint)
    {
    }

    bool DungeonMatchGroup::IsApproval() const
    {
        return std::ranges::all_of(_users | std::views::values, [](DungeonMatchGroupUserDecisionType type)
            {
                return type == DungeonMatchGroupUserDecisionType::Approve;
            });
    }

    bool DungeonMatchGroup::IsRejected() const
    {
        return std::ranges::any_of(_users | std::views::values, [](DungeonMatchGroupUserDecisionType type)
            {
                return type == DungeonMatchGroupUserDecisionType::Reject;
            });
    }

    bool DungeonMatchGroup::AddUser(coordination::game_user_id_type id)
    {
        return _users.try_emplace(id, DungeonMatchGroupUserDecisionType::Null).second;
    }

    auto DungeonMatchGroup::GetUserDecision(coordination::game_user_id_type id) const -> DungeonMatchGroupUserDecisionType
    {
        const auto iter = _users.find(id);
        assert(iter != _users.end());

        return iter->second;
    }

    bool DungeonMatchGroup::SetUserDecision(coordination::game_user_id_type id, DungeonMatchGroupUserDecisionType decision)
    {
        const auto iter = _users.find(id);
        if (iter == _users.end())
        {
            return false;
        }

        if (iter->second != DungeonMatchGroupUserDecisionType::Null)
        {
            return false;
        }

        iter->second = decision;

        return true;
    }

    auto DungeonMatchGroup::GetId() const -> int64_t
    {
        return _id;
    }

    auto DungeonMatchGroup::GetDungeonId() const -> int32_t
    {
        return _dungeonId;
    }

    auto DungeonMatchGroup::GetCreationTimePoint() const -> std::chrono::system_clock::time_point
    {
        return _creationTimePoint;
    }
}
