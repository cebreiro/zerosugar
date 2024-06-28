#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/xr/server/game/controller/game_entity_controller_interface.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_serializer.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_spawn.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_move.h"

using namespace zerosugar;
using namespace zerosugar::xr;

constexpr int64_t playerCount = 5000;

constexpr auto moveSyncPerSecond = 4;
constexpr auto simulationSecond = std::chrono::seconds(60);

constexpr int64_t moveTaskCount = (simulationSecond / std::chrono::seconds(1)) * 4;

constexpr auto positionRange = std::pair(0.f, 100000.f);

auto GetCharacterTemplate() -> const service::DTOCharacter&
{
    static service::DTOCharacter character{
        .characterId = -1,
        .name = "",
        .level = 0,
        .str = 0,
        .dex = 0,
        .intell = 0,
        .job = 0,
        .faceId = 0,
        .hairId = 0,
        .zoneId = 0,
        .x = 0.f,
        .y = 0.f,
        .z = 0.f,
        .items = {},
        .equipments = {}
    };

    return character;
}

class DummyController : public IGameController
{
public:
    bool IsSubscriberOf(int32_t opcode) const override
    {
        (void)opcode;

        return false;
    }

    void Notify(const IPacket& packet) override
    {
        (void)packet;
    }

    auto GetControllerId() const -> int64_t override
    {
        return _id;
    }

    void SetControllerId(int64_t id) override
    {
        _id = id;
    }

private:
    int64_t _id = 0;
};

auto MakeTestData(GameInstance& gameInstance) -> std::vector<std::pair<std::unique_ptr<GameTask>, int64_t>>
{
    std::chrono::system_clock::time_point simulationStartTime = std::chrono::system_clock::now();

    std::mt19937 mt(std::random_device{}());
    std::uniform_real_distribution dist1(positionRange.first, positionRange.second);
    std::uniform_real_distribution dist2(-5.f, 5.f);

    std::unordered_map<int64_t, std::vector<std::pair<std::unique_ptr<GameTask>, int64_t>>> tempMoveTasks;
    std::vector<std::pair<std::unique_ptr<GameTask>, int64_t>> result;
    result.reserve(playerCount + playerCount * moveTaskCount);

    for (int64_t i = 0; i < playerCount; ++i)
    {
        service::DTOCharacter character = GetCharacterTemplate();
        character.x = dist1(mt);
        character.y = dist1(mt);

        GameEntitySerializer serializer;
        SharedPtrNotNull<GameEntity> entity = serializer.Deserialize(character);

        auto controller = std::make_shared<DummyController>();
        controller->SetControllerId(gameInstance.PublishControllerId());

        const int64_t controllerId = controller->GetControllerId();
        const game_entity_id_type entityId = gameInstance.PublishPlayerId();

        entity->SetController(std::move(controller));
        entity->SetId(entityId);

        result.emplace_back(std::make_unique<game_task::PlayerSpawn>(std::move(entity), simulationStartTime), controllerId);

        auto& moveTasks = tempMoveTasks[i];
        moveTasks.reserve(moveTaskCount);

        for (int64_t j = 0; j < moveTaskCount; ++j)
        {
            std::chrono::system_clock::time_point moveSimulationTimePoint =
                simulationStartTime + (j * (std::chrono::seconds(1) / moveSyncPerSecond));

            auto param = std::make_unique<network::game::cs::MovePlayer>();
            param->position.x = character.x + dist2(mt);
            param->position.y = character.y + dist2(mt);

            character.x = param->position.x;
            character.y = param->position.y;

            moveTasks.emplace_back(std::make_unique<game_task::PlayerMove>(std::move(param), entityId, moveSimulationTimePoint), controllerId);
        }
    }

    for (int64_t j = 0; j < moveTaskCount; ++j)
    {
        for (int64_t i = 0; i < playerCount; ++i)
        {
            result.emplace_back(std::move(tempMoveTasks[i][j]));
        }
    }

    assert(std::ranges::all_of(result, [](const auto& pair)
        {
            return pair.first.operator bool();
        }));

    assert(std::ranges::is_sorted(result, [](const auto& lhs, const auto& rhs)
        {
            return lhs.first->GetCreationTimePoint() < rhs.first->GetCreationTimePoint();
        }));

    return result;
}

void TestDataBench(GameInstance& instance)
{
    ankerl::nanobench::Bench().run("test", [&]()
        {
            auto testData = MakeTestData(instance);
            ankerl::nanobench::doNotOptimizeAway(testData);
        });
}

void StartBench(GameInstance& instance)
{
    auto testData = MakeTestData(instance);

    ankerl::nanobench::Bench().run("test", [&]()
        {
            for (auto& [task, controllerId] : MakeTestData(instance))
            {
                assert(task);
                instance.Summit(std::move(task), controllerId);
            }

            testData.clear();

            while (instance.GetTaskScheduler().GetScheduledTaskCount() > 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
}

auto executor = std::make_shared<execution::AsioExecutor>(std::thread::hardware_concurrency());

int main()
{
    executor->Run();

    ExecutionContext::PushExecutor(executor.get());

    ServiceLocator locator;

    auto instance1 = std::make_shared<GameInstance>(executor, locator, game_instance_id_type{}, 0);
    TestDataBench(*instance1);

    auto instance2 = std::make_shared<GameInstance>(executor, locator, game_instance_id_type{}, 0);
    StartBench(*instance2);

    return 0;
}
