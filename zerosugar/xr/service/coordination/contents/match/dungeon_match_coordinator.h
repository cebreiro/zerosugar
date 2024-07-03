#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/service/coordination/contents/match/dungeon_match_group.h"
#include "zerosugar/xr/service/coordination/contents/match/dungeon_match_user.h"
#include "zerosugar/xr/service/coordination/node/node_id.h"

namespace zerosugar::xr
{
    class CoordinationService;
}

namespace zerosugar::xr::coordination
{
    class GameServer;
    class GameInstance;
}

namespace zerosugar::xr
{
    class DungeonMatchCoordinator final : public std::enable_shared_from_this<DungeonMatchCoordinator>
    {
    public:
        explicit DungeonMatchCoordinator(CoordinationService& coordinationService);
        ~DungeonMatchCoordinator();

        void Start();
        void Shutdown();
        auto Join() -> Future<void>;

        bool AddUser(coordination::game_user_id_type id, int32_t dungeonId);
        bool CancelUserMatch(coordination::game_user_id_type id);
        bool ApproveUserMatch(coordination::game_user_id_type id);
        bool RejectUserMatch(coordination::game_user_id_type id);

        void ForceRemoveUser(coordination::game_user_id_type id);

    private:
        auto Run() -> Future<void>;

        void RemoveCompleteTask();
        bool RemoveExpiredGroup();
        bool CreateMatchingGroup();

        void HandleMatchGroupCreation(const DungeonMatchGroup& group);
        void SendMatchGroupCreation(coordination::game_user_id_type id);

        auto HandleMatchGroupApproved(DungeonMatchGroup group) -> Future<void>;

        void HandleMatchGroupRejected(const DungeonMatchGroup& group);
        void SendMatchGroupRejected(coordination::game_user_id_type id);

        auto FindUser(coordination::game_user_id_type id) -> DungeonMatchUser*;
        auto FindGroup(int64_t id) -> DungeonMatchGroup*;
        auto FindServerWith(coordination::game_user_id_type id) -> coordination::GameServer*;

    private:
        CoordinationService& _coordinationService;
        bool _shutdown = false;
        Future<void> _runFuture;
        std::vector<Future<void>> _pending;

        int64_t _nextGroupId = 0;

        std::unordered_map<coordination::game_user_id_type, DungeonMatchUser> _matchUsers;
        boost::unordered::unordered_flat_map<int32_t, std::deque<PtrNotNull<DungeonMatchUser>>> _matchQueues;
        std::unordered_map<int64_t, DungeonMatchGroup> _matchGroups;
    };
}
