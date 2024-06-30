#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/xr/server/game/controller/game_controller_interface.h"
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

auto MakeTestData(GameInstance& gameInstance) -> std::vector<std::pair<std::unique_ptr<GameTask>, game_controller_id_type>>
{
    std::chrono::system_clock::time_point simulationStartTime = std::chrono::system_clock::now();

    std::mt19937 mt(std::random_device{}());
    std::uniform_real_distribution dist1(positionRange.first, positionRange.second);
    std::uniform_real_distribution dist2(-5.f, 5.f);

    std::unordered_map<int64_t, std::vector<std::pair<std::unique_ptr<GameTask>, game_controller_id_type>>> tempMoveTasks;
    std::vector<std::pair<std::unique_ptr<GameTask>, game_controller_id_type>> result;
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

        const game_controller_id_type controllerId = controller->GetControllerId();
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

void TestDataBench(GameInstance& instance, const std::string& executorName)
{
    ankerl::nanobench::Bench().run(std::format("{}) make simulation data", executorName), [&]()
        {
            auto testData = MakeTestData(instance);
            ankerl::nanobench::doNotOptimizeAway(testData);
        });
}

void StartBench(GameInstance& instance, const std::string& executorName)
{
    auto testData = MakeTestData(instance);

    ankerl::nanobench::Bench().run(std::format("{}) simulate movement", executorName), [&]()
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
                    PollingExecutor::Run(context);
                });
        }
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

        bool notify = false;
        {
            std::lock_guard lock(context.mutex);

            context.back.push_back(std::move(task));

            //notify = std::exchange(context.running, true) == false;
        }

        if (notify)
        {
            //context.condVar.notify_one();
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

    static void Run(Context& context)
    {
        while (true)
        {
            {
                std::unique_lock lock(context.mutex);

                assert(context.running);

                if (context.back.empty())
                {
                    context.running = false;

                    context.condVar.wait(lock, [&]()
                        {
                            return !context.back.empty() || context.shutdown;
                        });
                }

                context.front.swap(context.back);

                if (context.shutdown)
                {
                    return;
                }
            }

            assert(!context.front.empty());

            for (task_type& task : context.front)
            {
                std::visit([]<typename T>(T& item)
                {
                    item();

                    std::atomic_thread_fence(std::memory_order::release);

                }, task);
            }

            context.front.clear();
        }
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
auto gameExecutor = std::make_shared<PollingExecutor>(std::thread::hardware_concurrency());

int main()
{
    asioExecutor->Run();
    {
        ExecutionContext::ExecutorGuard guard(asioExecutor.get());

        ServiceLocator locator;

        auto instance1 = std::make_shared<GameInstance>(asioExecutor, locator, game_instance_id_type{}, 0);
        TestDataBench(*instance1, "asio executor");

        auto instance2 = std::make_shared<GameInstance>(asioExecutor, locator, game_instance_id_type{}, 0);
        StartBench(*instance2, "asio executor");
    }
    {
        ExecutionContext::ExecutorGuard guard(gameExecutor.get());

        ServiceLocator locator;

        auto instance1 = std::make_shared<GameInstance>(gameExecutor, locator, game_instance_id_type{}, 0);
        TestDataBench(*instance1, "poll executor");

        auto instance2 = std::make_shared<GameInstance>(gameExecutor, locator, game_instance_id_type{}, 0);
        StartBench(*instance2, "poll executor");
    }

    gameExecutor->Stop();

    return 0;
}
