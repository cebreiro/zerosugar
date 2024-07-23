#include "bot_control_service.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"
#include "zerosugar/shared/ai/behavior_tree/log/behavior_tree_log_service_adapter.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/register_task.h"

namespace zerosugar::xr
{
    BotControlService::BotControlService(const ServiceLocator& locator,
        SharedPtrNotNull<execution::AsioExecutor> ioExecutor, SharedPtrNotNull<execution::IExecutor> gameExecutor,
        int64_t concurrency, int64_t botCount, const std::string& btName)
        : _serviceLocator(locator)
        , _ioExecutor(std::move(ioExecutor))
        , _gameExecutor(std::move(gameExecutor))
        , _concurrency(std::max<int64_t>(1, concurrency))
        , _botControllers(std::max<int64_t>(1, botCount))
        , _behaviorTreeLogger(std::make_unique<BehaviorTreeLogServiceAdapter>(_serviceLocator.Get<ILogService>(), LogLevel::Debug))
        , _nodeSerializer(std::make_unique<bt::NodeSerializer>())
    {
        bot::RegisterTask(*_nodeSerializer);

        _sharedContexts.resize(_concurrency);
        _pingOperations.reserve(std::ssize(_botControllers));

        for (int64_t i = 0; i < concurrency; ++i)
        {
            _sharedContexts[i].naviStrand = std::make_shared<Strand>(_gameExecutor);
        }

        for (int64_t i = 0; i < std::ssize(_botControllers); ++i)
        {
            _botControllers[i] = std::make_shared<BotController>(_serviceLocator,
                std::make_shared<Strand>(_gameExecutor),
                _sharedContexts[i % concurrency],
                std::make_shared<Socket>(_ioExecutor->MakeStrand()),
                i,
                *_nodeSerializer,
                btName);

            _botControllers[i]->SetLogger(_behaviorTreeLogger.get());
            // _botControllers[i]->StartDebugOutputString();
        }
    }

    BotControlService::~BotControlService()
    {
    }

    void BotControlService::Start()
    {
        for (const SharedPtrNotNull<BotController>& botController : _botControllers)
        {
            botController->Start();
        }

        constexpr std::chrono::seconds pingDelay = std::chrono::seconds(15);

        Post(*_gameExecutor, [](WeakPtrNotNull<BotControlService> weak, std::chrono::seconds delay) -> Future<void>
            {
                while (true)
                {
                    co_await Delay(delay);

                    auto self = weak.lock();
                    if (!self)
                    {
                        co_return;
                    }

                    const RTTStatistics statistics = co_await self->EvaluateRTTStatistics();

                    ZEROSUGAR_LOG_INFO(self->_serviceLocator,
                        fmt::format("[{}] bot RTT statistics. min: {}ms, max: {}ms, average: {}ms",
                            self->GetName(), statistics.min, statistics.max, statistics.average));
                }

            }, weak_from_this(), pingDelay);
    }

    auto BotControlService::GetName() const -> std::string_view
    {
        return "bot_control_service";
    }

    auto BotControlService::EvaluateRTTStatistics() -> Future<RTTStatistics>
    {
        assert(_pingOperations.empty());
        assert(ExecutionContext::IsEqualTo(*_gameExecutor));

        const int64_t sequence = ++_pingSequence;

        for (const SharedPtrNotNull<BotController>& botController : _botControllers)
        {
            _pingOperations.emplace_back(botController->Ping(sequence));
        }

        co_await WaitAll(*_gameExecutor, _pingOperations);

        RTTStatistics statistics;
        int64_t i = 0;

        for (Future<std::optional<std::chrono::system_clock::duration>>& pingOp : _pingOperations)
        {
            assert(pingOp.IsComplete());

            const std::optional<std::chrono::system_clock::duration> pingResult = pingOp.Get();
            if (pingResult.has_value())
            {
                ++i;

                const int64_t count = std::chrono::duration_cast<std::chrono::milliseconds>(*pingResult).count();

                statistics.min = std::min(statistics.min, count);
                statistics.max = std::max(statistics.max, count);
                statistics.average += count;
            }
        }

        _pingOperations.clear();

        if (i == 0)
        {
            co_return RTTStatistics{};
        }

        statistics.average /= i;

        co_return statistics;
    }
}
