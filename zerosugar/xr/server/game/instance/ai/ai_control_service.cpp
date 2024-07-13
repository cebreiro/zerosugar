#include "ai_control_service.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/register_task.h"

namespace zerosugar::xr
{
    AIControlService::AIControlService(GameInstance& gameInstance)
        : _gameInstance(gameInstance)
        , _nodeSerializer(std::make_unique<bt::NodeSerializer>())
    {
        ai::RegisterTask(*_nodeSerializer);
    }

    AIControlService::~AIControlService()
    {
    }

    auto AIControlService::ShutdownAndJoin() -> Future<void>
    {
        if (!ExecutionContext::IsEqualTo(_gameInstance.GetStrand()))
        {
            co_await _gameInstance.GetStrand();
        }

        std::vector<Future<void>> futures;
        futures.reserve(_controllers.size());

        for (const SharedPtrNotNull<AIController>& controller : _controllers | std::views::values)
        {
            futures.push_back(DeleteAIController(controller->GetId()));
        }

        co_await WaitAll(_gameInstance.GetStrand(), futures);

        assert(_controllers.empty());

        co_return;
    }

    bool AIControlService::Contains(game_controller_id_type id) const
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        return _controllers.contains(id);
    }

    auto AIControlService::CreateAIController(game_entity_id_type entityId, const std::string& btName) -> SharedPtrNotNull<AIController>
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        const game_controller_id_type controllerId = _gameInstance.PublishControllerId();
        auto controller = std::make_shared<AIController>(_gameInstance, *_nodeSerializer, controllerId, entityId, btName);

        [[maybe_unused]]
        const bool inserted = _controllers.try_emplace(controllerId, controller).second;
        assert(inserted);

        (void)_aiControllerEntityIndex.try_emplace(entityId, controller.get());

        return controller;
    }

    auto AIControlService::DeleteAIController(game_controller_id_type id) -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        const auto iter = _controllers.find(id);
        if (iter == _controllers.end())
        {
            co_return;
        }

        const SharedPtrNotNull<AIController>& controller = iter->second;
        if (!controller->IsRunning())
        {
            co_return;
        }

        const game_entity_id_type entityId = controller->GetEntityId();

        controller->Shutdown();
        co_await controller->Join();

        [[maybe_unused]]
        const size_t count = _controllers.erase(id);
        assert(count > 0);

        (void)_aiControllerEntityIndex.erase(entityId);

        co_return;
    }

    auto AIControlService::FindAIController(game_entity_id_type entityId) -> AIController*
    {
        const auto iter = _aiControllerEntityIndex.find(entityId);

        return iter != _aiControllerEntityIndex.end() ? iter->second : nullptr;
    }
}
