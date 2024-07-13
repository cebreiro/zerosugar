#include "dungeon_match_coordinator.h"

#include "zerosugar/xr/service/coordination/coordination_service.h"
#include "zerosugar/xr/service/coordination/load_balance/load_balancer_interface.h"
#include "zerosugar/xr/service/coordination/node/game_instance.h"
#include "zerosugar/xr/service/coordination/node/game_server.h"
#include "zerosugar/xr/service/coordination/node/game_user.h"
#include "zerosugar/xr/service/coordination/node/node_container.h"
#include "zerosugar/xr/service/model/generated/coordination_command_message.h"

namespace zerosugar::xr
{
    DungeonMatchCoordinator::DungeonMatchCoordinator(CoordinationService& coordinationService)
        : _coordinationService(coordinationService)
    {
    }

    DungeonMatchCoordinator::~DungeonMatchCoordinator()
    {
    }

    void DungeonMatchCoordinator::Start()
    {
        assert(!_runFuture.IsValid());

        Post(_coordinationService.GetStrand(), [self = shared_from_this()]()
            {
                self->Run();
            });
    }

    void DungeonMatchCoordinator::Shutdown()
    {
        _shutdown = true;
    }

    auto DungeonMatchCoordinator::Join() -> Future<void>
    {
        assert(_runFuture.IsValid());

        co_await _runFuture;

        for (Future<void>& future : _pending)
        {
            co_await future;
        }

        _runFuture = Future<void>();
        _pending.clear();

        co_return;
    }

    bool DungeonMatchCoordinator::AddUser(coordination::game_user_id_type id, int32_t dungeonId)
    {
        assert(ExecutionContext::IsEqualTo(_coordinationService.GetStrand()));

        const auto& [iter, inserted] = _matchUsers.try_emplace(id, DungeonMatchUser(id, dungeonId, std::chrono::system_clock::now()));

        if (inserted)
        {
            PtrNotNull<DungeonMatchUser> user = &iter->second;
            std::deque<PtrNotNull<DungeonMatchUser>>& matchQueue = _matchQueues[dungeonId];

            matchQueue.push_back(user);

            return true;
        }

        return false;
    }

    bool DungeonMatchCoordinator::CancelUserMatch(coordination::game_user_id_type id)
    {
        assert(ExecutionContext::IsEqualTo(_coordinationService.GetStrand()));

        const auto iter = _matchUsers.find(id);
        if (iter == _matchUsers.end())
        {
            return false;
        }

        PtrNotNull<DungeonMatchUser> user = &iter->second;
        if (user->GetState() == DungeonMatchUserState::Grouped)
        {
            return false;
        }

        std::deque<PtrNotNull<DungeonMatchUser>>& matchQueue = _matchQueues[user->GetDungeonId()];

        auto matchQueueIter = std::ranges::find(matchQueue, user);
        if (matchQueueIter == matchQueue.end())
        {
            return false;
        }

        matchQueue.erase(matchQueueIter);
        _matchUsers.erase(iter);

        return true;
    }

    bool DungeonMatchCoordinator::ApproveUserMatch(coordination::game_user_id_type id)
    {
        assert(ExecutionContext::IsEqualTo(_coordinationService.GetStrand()));

        const DungeonMatchUser* user = FindUser(id);
        if (!user)
        {
            return false;
        }

        if (user->GetState() != DungeonMatchUserState::Grouped)
        {
            return false;
        }

        const std::optional<int64_t> groupId = user->GetGroupId();
        assert(groupId.has_value());

        DungeonMatchGroup* group = FindGroup(*groupId);
        if (!groupId)
        {
            return false;
        }

        if (group->SetUserDecision(id, DungeonMatchGroupUserDecisionType::Approve))
        {
            if (group->IsApproval())
            {
                Future<void> future = HandleMatchGroupApproved(std::move(*group));
                _matchGroups.erase(*groupId);

                _pending.push_back(std::move(future));
            }

            return true;
        }

        return false;
    }

    bool DungeonMatchCoordinator::RejectUserMatch(coordination::game_user_id_type id)
    {
        assert(ExecutionContext::IsEqualTo(_coordinationService.GetStrand()));

        const DungeonMatchUser* user = FindUser(id);
        if (!user)
        {
            return false;
        }

        if (user->GetState() != DungeonMatchUserState::Grouped)
        {
            return false;
        }

        const std::optional<int64_t> groupId = user->GetGroupId();
        assert(groupId.has_value());

        DungeonMatchGroup* group = FindGroup(*groupId);
        if (!groupId)
        {
            return false;
        }

        if (group->SetUserDecision(id, DungeonMatchGroupUserDecisionType::Reject))
        {
            if (group->IsRejected())
            {
                HandleMatchGroupRejected(*group);
                _matchGroups.erase(*groupId);
            }

            return true;
        }

        return false;
    }

    void DungeonMatchCoordinator::ForceRemoveUser(coordination::game_user_id_type id)
    {
        assert(ExecutionContext::IsEqualTo(_coordinationService.GetStrand()));

        (void)id;
    }

    auto DungeonMatchCoordinator::Run() -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(_coordinationService.GetStrand()));

        constexpr auto maxTickInterval = std::chrono::milliseconds(1000);
        constexpr auto minTickInterval = std::chrono::milliseconds(1);
        auto tickInterval = minTickInterval;

        while (true)
        {
            co_await Delay(tickInterval);
            assert(ExecutionContext::IsEqualTo(_coordinationService.GetStrand()));

            if (_shutdown)
            {
                co_return;
            }

            RemoveCompleteTask();

            bool processed = false;
            processed |= RemoveExpiredGroup();
            processed |= CreateMatchingGroup();

            if (processed)
            {
                tickInterval = std::max(minTickInterval, tickInterval / 4);
            }
            else
            {
                tickInterval = std::min(maxTickInterval, tickInterval * 2);
            }
        }
    }

    void DungeonMatchCoordinator::RemoveCompleteTask()
    {
        for (auto iter = _pending.begin(); iter != _pending.end();)
        {
            if (!iter->IsPending())
            {
                iter = _pending.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    bool DungeonMatchCoordinator::RemoveExpiredGroup()
    {
        int64_t removeCount = 0;

        const auto now = std::chrono::system_clock::now();
        constexpr auto groupExpiredInterval = std::chrono::seconds(30);

        for (auto iter = _matchGroups.begin(); iter != _matchGroups.end(); )
        {
            if (now - iter->second.GetCreationTimePoint() >= groupExpiredInterval)
            {
                ++removeCount;

                HandleMatchGroupRejected(iter->second);
                iter = _matchGroups.erase(iter);
            }
            else
            {
                ++iter;
            }
        }

        return removeCount > 0;
    }

    bool DungeonMatchCoordinator::CreateMatchingGroup()
    {
        int64_t makingCount = 0;

        for (auto& [dungeonId, matchQueue] : _matchQueues)
        {
            constexpr int64_t matchOrganizeSize = 5;

            while (std::ssize(matchQueue) >= matchOrganizeSize)
            {
                const int64_t groupId = ++_nextGroupId;
                DungeonMatchGroup group(groupId, dungeonId, std::chrono::system_clock::now());

                for (int64_t i = 0; i < matchOrganizeSize; ++i)
                {
                    PtrNotNull<DungeonMatchUser> user = matchQueue.front();
                    matchQueue.pop_front();

                    assert(user->GetState() == DungeonMatchUserState::Waiting);
                    assert(user->GetGroupId() == std::nullopt);

                    user->SetState(DungeonMatchUserState::Grouped);
                    user->SetGroupId(group.GetId());

                    [[maybe_unused]]
                    const bool added = group.AddUser(user->GetId());
                    assert(added);
                }

                [[maybe_unused]]
                const auto& [iter, inserted] = _matchGroups.try_emplace(groupId, std::move(group));
                assert(inserted);

                ++makingCount;

                HandleMatchGroupCreation(iter->second);
            }
        }

        return makingCount > 0;
    }

    void DungeonMatchCoordinator::HandleMatchGroupCreation(const DungeonMatchGroup& group)
    {
        for (coordination::game_user_id_type id : group.GetGameUserIdRange())
        {
            SendMatchGroupCreation(id);
        }
    }

    void DungeonMatchCoordinator::SendMatchGroupCreation(coordination::game_user_id_type id)
    {
        coordination::GameServer* gameServer = FindServerWith(id);
        assert(gameServer);

        coordination::command::NotifyDungeonMatchGroupCreation command;
        command.userId = id.Unwrap();

        gameServer->SendCommand(command);
    }

    auto DungeonMatchCoordinator::HandleMatchGroupApproved(DungeonMatchGroup group) -> Future<void>
    {
        coordination::GameServer* dungeonServer = _coordinationService.GetLoadBalancer().Select();
        assert(dungeonServer);

        const coordination::game_instance_id_type instanceId = _coordinationService.PublishGameInstanceId();

        coordination::command::LaunchGameInstance launchGameInstance;
        launchGameInstance.zoneId = group.GetDungeonId();
        launchGameInstance.gameInstanceId = instanceId.Unwrap();

        Future<void> completionToken;
        dungeonServer->SendCommand(launchGameInstance, completionToken);

        co_await completionToken;

        coordination::NodeContainer& nodeContainer = _coordinationService.GetNodeContainer();
        coordination::GameInstance* dungeon = nodeContainer.Find(instanceId);
        assert(dungeon);

        for (coordination::game_user_id_type id : group.GetGameUserIdRange())
        {
            [[maybe_unused]]
            const size_t count = _matchUsers.erase(id);
            assert(count > 0);

            coordination::GameUser* user = nodeContainer.Find(id);
            if (!user)
            {
                continue;
            }

            coordination::GameInstance* oldGameInstance = user->GetParent();

            oldGameInstance->RemoveChild(id);
            dungeon->AddChild(id, user);
            user->SetParent(dungeon);

            user->SetMigrating(true);

            coordination::GameServer* oldServer = oldGameInstance->GetParent();
            coordination::GameServer* newServer = dungeon->GetParent();
            assert(oldServer && newServer);

            coordination::command::NotifyDungeonMatchGroupApproved command;
            command.userId = id.Unwrap();
            command.ip = newServer->GetIP();
            command.port = newServer->GetPort();

            oldServer->SendCommand(command);
        }
    }

    void DungeonMatchCoordinator::HandleMatchGroupRejected(const DungeonMatchGroup& group)
    {
        for (coordination::game_user_id_type id : group.GetGameUserIdRange())
        {
            [[maybe_unused]]
            const size_t count = _matchUsers.erase(id);
            assert(count > 0);

            //const auto iter = _matchUsers.find(id);
            //assert(iter != _matchUsers.end());

            //PtrNotNull<DungeonMatchUser> user = &iter->second;

            //user->SetGroupId(std::nullopt);
            //user->SetState(DungeonMatchUserState::Waiting);

            //if (group.GetUserDecision(id) != DungeonMatchGroupUserDecisionType::Reject)
            //{
            //    std::deque<DungeonMatchUser*>& matchQueue = _matchQueues[group.GetDungeonId()];

            //    matchQueue.push_front(user);
            //}

            SendMatchGroupRejected(id);
        }
    }

    void DungeonMatchCoordinator::SendMatchGroupRejected(coordination::game_user_id_type id)
    {
        coordination::GameServer* gameServer = FindServerWith(id);
        assert(gameServer);

        coordination::command::NotifyDungeonMatchGroupRejected command;
        command.userId = id.Unwrap();

        gameServer->SendCommand(command);
    }

    auto DungeonMatchCoordinator::FindUser(coordination::game_user_id_type id) -> DungeonMatchUser*
    {
        const auto iter = _matchUsers.find(id);

        return iter != _matchUsers.end() ? &iter->second : nullptr;
    }

    auto DungeonMatchCoordinator::FindGroup(int64_t id) -> DungeonMatchGroup*
    {
        const auto iter = _matchGroups.find(id);

        return iter != _matchGroups.end() ? &iter->second : nullptr;
    }

    auto DungeonMatchCoordinator::FindServerWith(coordination::game_user_id_type id) -> coordination::GameServer*
    {
        coordination::GameUser* user = _coordinationService.GetNodeContainer().Find(id);
        if (!user)
        {
            return nullptr;
        }

        coordination::GameInstance* gameInstance = user->GetParent();
        assert(gameInstance);

        coordination::GameServer* gameServer = gameInstance->GetParent();
        assert(gameServer);

        return gameServer;
    }
}
