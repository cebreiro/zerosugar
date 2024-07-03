#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/service/coordination/node/node_id.h"

namespace zerosugar::xr
{
    class DungeonMatchUser;
}

namespace zerosugar::xr
{
    ENUM_CLASS(DungeonMatchGroupUserDecisionType, int32_t,
        (Null)
        (Approve)
        (Reject)
    )

    class DungeonMatchGroup
    {
    public:
        DungeonMatchGroup() = default;
        DungeonMatchGroup(int64_t id, int32_t dungeonId, std::chrono::system_clock::time_point creationTimePoint);

        bool IsApproval() const;
        bool IsRejected() const;

        bool AddUser(coordination::game_user_id_type id);
        auto GetUserDecision(coordination::game_user_id_type id) const -> DungeonMatchGroupUserDecisionType;

        bool SetUserDecision(coordination::game_user_id_type id, DungeonMatchGroupUserDecisionType decision);

        auto GetId() const -> int64_t;
        auto GetDungeonId() const -> int32_t;
        auto GetCreationTimePoint() const ->std::chrono::system_clock::time_point;

        inline auto GetGameUserIdRange() const;

    private:
        int64_t _id = 0;
        int32_t _dungeonId = 0;
        std::chrono::system_clock::time_point _creationTimePoint;

        boost::unordered::unordered_flat_map<coordination::game_user_id_type, DungeonMatchGroupUserDecisionType> _users;
    };

    auto DungeonMatchGroup::GetGameUserIdRange() const
    {
        return _users | std::views::keys;
    }
}
