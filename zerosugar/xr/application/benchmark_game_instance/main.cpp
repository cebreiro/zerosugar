#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/impl/game_executor.h"
#include "zerosugar/xr/data/game_data_provider.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_serializer.h"
#include "zerosugar/xr/server/game/instance/entity/component/inventory_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/player_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_spawn.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_move.h"

using namespace zerosugar;
using namespace zerosugar::xr;

constexpr int64_t playerCount = 3000;

constexpr auto moveSyncPerSecond = 4;
constexpr auto simulationSecond = std::chrono::seconds(10);

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
        .items = {},
        .equipments = {}
    };

    return character;
}

class DummyController : public IGameController
{
public:
    void Notify(const IPacket& packet) override
    {
        (void)packet;
    }

    auto GetControllerId() const -> game_controller_id_type override
    {
        return _id;
    }

    void SetControllerId(game_controller_id_type id) override
    {
        _id = id;
    }

private:
    game_controller_id_type _id;
};

class TestEntitySerializer : public IGameEntitySerializer
{
public:
    TestEntitySerializer() = delete;
    TestEntitySerializer(double x, double y)
        : _x(x)
        , _y(y)
    {
    }

    auto Serialize(const GameEntity& entity) const -> service::DTOCharacter override
    {
        (void)entity;

        return {};
    }

    auto Deserialize(const service::DTOCharacter& character) const -> SharedPtrNotNull<GameEntity> override
    {
        auto entity = std::make_shared<GameEntity>();
        entity->AddComponent(std::make_unique<PlayerComponent>(character));
        entity->AddComponent(std::make_unique<MovementComponent>());
        entity->AddComponent(std::make_unique<InventoryComponent>());
        entity->AddComponent(std::make_unique<StatComponent>());

        {
            auto& inventory = entity->GetComponent<InventoryComponent>();

            [[maybe_unused]]
            const bool success = inventory.Initialize(character.characterId, character.items, character.equipments);
            assert(success);
        }
        {
            auto& movementComponent = entity->GetComponent<MovementComponent>();
            movementComponent.SetPosition(Eigen::Vector3d(_x, _y, 0));
        }

        return entity;
    }

private:
    double _x = 0.0;
    double _y = 0.0;
};

struct TestData
{
    std::vector<std::pair<std::unique_ptr<GameTask>, game_controller_id_type>> spawnTasks;
    std::vector<std::pair<std::unique_ptr<GameTask>, game_controller_id_type>> moveTasks;
};

auto MakeTestData(GameInstance& gameInstance, int32_t seed) -> TestData
{
    std::chrono::system_clock::time_point simulationStartTime = std::chrono::system_clock::now();

    std::mt19937 mt(seed);
    std::uniform_real_distribution dist1(positionRange.first, positionRange.second);
    std::uniform_real_distribution dist2(-5.f, 5.f);

    TestData result;

    std::unordered_map<int64_t, std::vector<std::pair<std::unique_ptr<GameTask>, game_controller_id_type>>> tempMoveTasks;
    result.spawnTasks.reserve(playerCount);
    result.moveTasks.reserve(playerCount * moveTaskCount);

    for (int64_t i = 0; i < playerCount; ++i)
    {
        service::DTOCharacter character = GetCharacterTemplate();

        double posX = dist1(mt);
        double posY = dist1(mt);

        TestEntitySerializer serializer(posX, posY);
        SharedPtrNotNull<GameEntity> entity = serializer.Deserialize(character);

        auto controller = std::make_shared<DummyController>();
        controller->SetControllerId(gameInstance.PublishControllerId());

        const game_controller_id_type controllerId = controller->GetControllerId();
        const game_entity_id_type entityId = gameInstance.PublishEntityId(GameEntityType::Player);

        entity->SetController(std::move(controller));
        entity->SetId(entityId);

        result.spawnTasks.emplace_back(std::make_unique<game_task::PlayerSpawn>(std::move(entity), simulationStartTime), controllerId);

        auto& moveTasks = tempMoveTasks[i];
        moveTasks.reserve(moveTaskCount);

        for (int64_t j = 0; j < moveTaskCount; ++j)
        {
            std::chrono::system_clock::time_point moveSimulationTimePoint =
                simulationStartTime + (j * (std::chrono::seconds(1) / moveSyncPerSecond));

            auto param = std::make_unique<network::game::cs::MovePlayer>();
            param->position.x = (float)posX + dist2(mt);
            param->position.y = (float)posY + dist2(mt);

            posX = param->position.x;
            posY = param->position.y;

            moveTasks.emplace_back(std::make_unique<game_task::PlayerMove>(std::move(param), entityId, moveSimulationTimePoint), controllerId);
        }
    }

    for (int64_t j = 0; j < moveTaskCount; ++j)
    {
        for (int64_t i = 0; i < playerCount; ++i)
        {
            result.moveTasks.emplace_back(std::move(tempMoveTasks[i][j]));
        }
    }

    return result;
}

std::vector<int32_t> seeds;

void TestDataBench(GameInstance& instance, const std::string& executorName)
{
    size_t i = 0;

    ankerl::nanobench::Bench().run(fmt::format("{}) make simulation data", executorName), [&]()
        {
            auto testData = MakeTestData(instance, seeds[i++]);
            ankerl::nanobench::doNotOptimizeAway(testData);
        });
}

void StartBench(GameInstance& instance, const std::string& executorName)
{
    size_t i = 0;

    ankerl::nanobench::Bench().run(fmt::format("{}) simulate movement", executorName), [&]()
        {
            auto data = MakeTestData(instance, seeds[i++]);

            const int64_t spawnTaskSize = std::ssize(data.spawnTasks);

            for (auto& [task, controllerId] : data.spawnTasks)
            {
                assert(task);
                instance.Summit(std::move(task), controllerId);
            }

            while (instance.GetTaskScheduler().GetCompleteTaskCount() < spawnTaskSize)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            instance.GetTaskScheduler().ResetCompletionTaskCount();

            for (auto& [task, controllerId] : data.moveTasks)
            {
                assert(task);
                instance.Summit(std::move(task), controllerId);
            }

            const int64_t moveTaskSize = std::ssize(data.moveTasks);

            while (instance.GetTaskScheduler().GetCompleteTaskCount() < moveTaskSize)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            instance.GetTaskScheduler().ResetCompletionTaskCount();
        }).minEpochIterations(100);
}

class PollingExecutor final : public execution::IExecutor, public std::enable_shared_from_this<PollingExecutor>
{
private:
    using task_type = std::variant<std::function<void()>, std::move_only_function<void()>>;

    struct Context
    {
        std::jthread worker;

        std::condition_variable condVar;

        std::mutex mutex;
        bool running = false;
        bool shutdown = false;
        std::vector<task_type> front;
        std::vector<task_type> back;
    };

public:
    explicit PollingExecutor(int64_t workerCount)
        : _workerCount(std::max<int64_t>(1, workerCount))
    {
        for (int64_t i = 0; i < workerCount; ++i)
        {
            Context& context = *_workContexts.emplace_back(std::make_unique<Context>());

            context.worker = std::jthread([&context]()
                {
                    PollingExecutor::RunPoll(context);
                });
        }
    }

    ~PollingExecutor()
    {
        Stop();
    }

    void Stop() override
    {
        for (auto& work : _workContexts)
        {
            {
                std::lock_guard lock(work->mutex);

                work->shutdown = true;
            }

            work->condVar.notify_one();
        }

        _workContexts.clear();
    }

    void Post(const std::function<void()>& function) override
    {
        Post(task_type(function));
    }

    void Post(std::move_only_function<void()> function) override
    {
        Post(task_type(std::move(function)));
    }

    void Dispatch(const std::function<void()>& function) override
    {
        Dispatch(task_type(function));
    }

    void Dispatch(std::move_only_function<void()> function) override
    {
        Dispatch(task_type(std::move(function)));
    }

    auto GetConcurrency() const -> int64_t override
    {
        return _workerCount;
    }

    auto SharedFromThis() -> SharedPtrNotNull<IExecutor> override
    {
        return shared_from_this();
    }

    auto SharedFromThis() const -> SharedPtrNotNull<const IExecutor> override
    {
        return shared_from_this();
    }

private:
    void Post(task_type task)
    {
        Context& context = *_workContexts[_count.fetch_add(1) % std::ssize(_workContexts)];

        {
            std::lock_guard lock(context.mutex);

            context.back.push_back(std::move(task));
        }
    }

    void Dispatch(task_type task)
    {
        if (ExecutionContext::GetExecutor() == this)
        {
            std::visit([]<typename T>(T& item)
            {
                item();

                std::atomic_thread_fence(std::memory_order::release);

            }, task);

            return;
        }

        Post(std::move(task));
    }

    static void RunPoll(Context& context)
    {
        while (true)
        {
            while (true)
            {
                std::unique_lock lock(context.mutex);

                if (context.shutdown)
                {
                    return;
                }

                if (context.back.empty())
                {
                    lock.unlock();

                    std::this_thread::yield();
                }
                else
                {
                    context.front.swap(context.back);

                    break;
                }
            }

            assert(!context.front.empty());

            for (task_type& task : context.front)
            {
                std::visit([]<typename T>(T & item)
                {
                    item();

                    std::atomic_thread_fence(std::memory_order::release);

                }, task);
            }

            context.front.clear();
        }
    }

private:
    int64_t _workerCount = 0;
    std::atomic<int64_t> _count = 0;
    std::vector<std::unique_ptr<Context>> _workContexts;
};

auto asioExecutor = std::make_shared<execution::AsioExecutor>(std::thread::hardware_concurrency());
auto gameExecutor = std::make_shared<execution::GameExecutor>(std::thread::hardware_concurrency());

int main()
{
    constexpr int64_t seedSize = 10000;
    seeds.resize(seedSize);

    std::random_device rnd;

    for (int64_t i = 0; i < seedSize; ++i)
    {
        seeds[i] = rnd();
    }

    asioExecutor->Run();
    {
        ExecutionContext::ExecutorGuard guard(asioExecutor.get());

        ServiceLocator locator;

        std::shared_ptr<GameDataProvider> gameDataProvider = std::make_shared<GameDataProvider>();
        gameDataProvider->Initialize(locator);

        locator.Add<GameDataProvider>(gameDataProvider);

        auto instance1 = std::make_shared<GameInstance>(asioExecutor, locator, game_instance_id_type{}, 100);
        TestDataBench(*instance1, "asio executor");

        instance1->Shutdown();
        instance1->Join();

        auto instance2 = std::make_shared<GameInstance>(asioExecutor, locator, game_instance_id_type{}, 100);
        StartBench(*instance2, "asio executor");

        instance2->Shutdown();
        instance2->Join();
    }

    asioExecutor->Stop();

    auto pollExecutor = std::make_shared<PollingExecutor>(std::thread::hardware_concurrency());
    {
        ExecutionContext::ExecutorGuard guard(pollExecutor.get());

        ServiceLocator locator;

        std::shared_ptr<GameDataProvider> gameDataProvider = std::make_shared<GameDataProvider>();
        gameDataProvider->Initialize(locator);

        locator.Add<GameDataProvider>(gameDataProvider);

        auto instance1 = std::make_shared<GameInstance>(pollExecutor, locator, game_instance_id_type{}, 100);
        TestDataBench(*instance1, "poll executor");

        instance1->Shutdown();
        instance1->Join();

        auto instance2 = std::make_shared<GameInstance>(pollExecutor, locator, game_instance_id_type{}, 100);
        StartBench(*instance2, "poll executor");

        instance2->Shutdown();
        instance2->Join();
    }
    pollExecutor->Stop();

    gameExecutor->Run();
    {
        ExecutionContext::ExecutorGuard guard(gameExecutor.get());

        ServiceLocator locator;

        std::shared_ptr<GameDataProvider> gameDataProvider = std::make_shared<GameDataProvider>();
        gameDataProvider->Initialize(locator);

        locator.Add<GameDataProvider>(gameDataProvider);

        auto instance1 = std::make_shared<GameInstance>(gameExecutor, locator, game_instance_id_type{}, 100);
        TestDataBench(*instance1, "game executor");

        instance1->Shutdown();
        instance1->Join();

        auto instance2 = std::make_shared<GameInstance>(gameExecutor, locator, game_instance_id_type{}, 100);
        StartBench(*instance2, "work stealing executor");

        instance2->Shutdown();
        instance2->Join();
    }

    gameExecutor->Stop();
    

    return 0;
}
