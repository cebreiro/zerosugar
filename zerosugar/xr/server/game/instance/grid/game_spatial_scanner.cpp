#include "game_spatial_scanner.h"

#include <boost/scope/scope_exit.hpp>
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_sector.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_mbr.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"

namespace zerosugar::xr
{
    GameSpatialScanner::GameSpatialScanner(GameInstance& gameInstance)
        : _gameInstance(gameInstance)
    {
    }

    void GameSpatialScanner::Shutdown()
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        _shutdown = true;
    }

    auto GameSpatialScanner::Join() -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        if (!_running)
        {
            co_return;
        }

        assert(_runFuture.IsValid());

        co_await _runFuture;

        _runFuture = Future<void>();
        _running = false;

        co_return;
    }

    void GameSpatialScanner::Schedule(game_entity_id_type id, double radius, std::initializer_list<GameEntityType> targetTypes)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        Task task;
        task.id = id;
        task.radius = radius;
        std::ranges::copy(targetTypes, std::back_inserter(task.targetTypes));

        if (!_recycleBuffer.empty())
        {
            task.result.swap(_recycleBuffer.back());
            _recycleBuffer.pop_back();
        }

        if (!_tasks.try_emplace(id, task).second)
        {
            assert(false);

            return;
        }

        if (!_running)
        {
            Post(_gameInstance.GetStrand(), [this]()
                {
                    _runFuture = Run();
                });
        }
    }

    void GameSpatialScanner::SetSignalHandler(game_entity_id_type id, const std::function<void()>& signalHandler)
    {
        assert(_tasks.contains(id));
        assert(!_signals.contains(id));

        _signals[id] = signalHandler;
    }

    void GameSpatialScanner::CancelScheduled(game_entity_id_type id)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        if (const auto iter = _tasks.find(id); iter != _tasks.end())
        {
            iter->second.result.clear();
            _recycleBuffer.push_back(std::move(iter->second.result));

            _tasks.erase(iter);
        }
        else
        {
            assert(false);
        }
    }

    auto GameSpatialScanner::GetLastScanResult(game_entity_id_type id) const -> const boost::container::flat_set<game_entity_id_type>&
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        if (const auto iter = _tasks.find(id); iter != _tasks.end())
        {
            return iter->second.result;
        }

        static const boost::container::flat_set<game_entity_id_type> nullObject;

        return nullObject;
    }

    auto GameSpatialScanner::TakeScanResult(game_entity_id_type id) -> boost::container::flat_set<game_entity_id_type>
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        if (const auto iter = _tasks.find(id); iter != _tasks.end())
        {
            return std::move(iter->second.result);
        }

        return {};
    }

    void GameSpatialScanner::TakeBackBuffer(boost::container::flat_set<game_entity_id_type> buffer)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        buffer.clear();

        _recycleBuffer.push_back(std::move(buffer));
    }

    void GameSpatialScanner::SetScanInterval(std::chrono::milliseconds interval)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        _scanInterval = interval;
    }

    auto GameSpatialScanner::Run() -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        while (true)
        {
            Scan();

            co_await Delay(_scanInterval);
            assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

            if (_shutdown || _tasks.empty())
            {
                break;
            }
        }

        _running = false;

        co_return;
    }

    void GameSpatialScanner::Scan()
    {
        assert(!_tasks.empty());

        const GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        const GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();

        boost::scope::scope_exit exit([this]()
            {
                _batchTasks.clear();
            });

        for (auto iter = _tasks.begin(); iter != _tasks.end(); )
        {
            Task& task = iter->second;

            if (const std::optional<Eigen::Vector3d>& pos = snapshotContainer.FindPosition(task.id);
                pos.has_value())
            {
                task.result.clear();

                const GameSpatialSector& sector = spatialContainer.GetSector(*pos);

                BatchTask& batch = _batchTasks[sector.GetId()];
                batch.sector = &spatialContainer.GetSector(*pos);

                const Eigen::Vector2d pos2d(pos->x(), pos->y());

                const int64_t quadIndex = sector.GetCenter().GetQuadIndex(pos2d);
                assert(quadIndex >= 0 && quadIndex < std::ssize(batch.quadPartition));

                std::optional<BatchTask::Value>& quad = batch.quadPartition[quadIndex];
                if (!quad.has_value())
                {
                    quad.emplace();
                }

                quad->mbr.Extend(GameSpatialMBR(pos2d, task.radius));
                quad->items.emplace_back(&task, pos2d);

                ++iter;
            }
            else
            {
                iter = _tasks.erase(iter);
            }
        }

        const auto filter = [](const std::optional<BatchTask::Value>& opt)
            {
                return opt.has_value();
            };
        const auto transform = [](const std::optional<BatchTask::Value>& opt) -> const BatchTask::Value&
            {
                return opt.value();
            };

        for (const BatchTask& batch : _batchTasks | std::views::values)
        {
            for (const BatchTask::Value& value : batch.quadPartition | std::views::filter(filter) | std::views::transform(transform))
            {
                for (const GameSpatialCell& cell : batch.sector->GetCells())
                {
                    if (!value.mbr.Intersect(cell.GetMBR()))
                    {
                        continue;
                    }

                    for (const auto& [task, position] : value.items)
                    {
                        if (!GameSpatialMBR(position, task->radius).Intersect(cell.GetMBR()))
                        {
                            continue;
                        }

                        for (const GameEntityType type : task->targetTypes)
                        {
                            if (!cell.HasEntity(type))
                            {
                                continue;
                            }

                            const auto distanceSq = (task->radius) * task->radius;

                            for (const game_entity_id_type id : cell.GetEntities(type))
                            {
                                const std::optional<Eigen::Vector3d> otherPos = snapshotContainer.FindPosition(id);
                                assert(otherPos);

                                const Eigen::Vector2d otherPos2d(otherPos->x(), otherPos->y());

                                if ((position - otherPos2d).squaredNorm() <= distanceSq)
                                {
                                    task->result.emplace(id);
                                }
                            }
                        }
                    }
                }
            }
        }

        for (const std::function<void()>& signal : _signals | std::views::values)
        {
            assert(signal);

            signal();
        }

        _signals.clear();
    }
}
