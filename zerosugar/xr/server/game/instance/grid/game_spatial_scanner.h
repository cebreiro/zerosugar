#pragma once
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container/small_vector.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_id.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_mbr.h"

namespace zerosugar::xr
{
    class GameInstance;
    class GameSpatialSector;
}

namespace zerosugar::xr
{
    class GameSpatialScanner
    {
    public:
        explicit GameSpatialScanner(GameInstance& gameInstance);

        void Shutdown();
        auto Join() -> Future<void>;

        void Schedule(game_entity_id_type id, double radius, std::initializer_list<GameEntityType> targetTypes);
        void SetSignalHandler(game_entity_id_type id, const std::function<void()>& signalHandler);
        void CancelScheduled(game_entity_id_type id);

        auto GetLastScanResult(game_entity_id_type id) const -> const boost::container::flat_set<game_entity_id_type>&;

        auto TakeScanResult(game_entity_id_type id) -> boost::container::flat_set<game_entity_id_type>;
        void TakeBackBuffer(boost::container::flat_set<game_entity_id_type> buffer);

        void SetScanInterval(std::chrono::milliseconds interval);

    private:
        auto Run() -> Future<void>;
        void Scan();

    private:
        struct Task
        {
            game_entity_id_type id;
            boost::container::flat_set<game_entity_id_type> result;

            double radius = 0.0;
            boost::container::static_vector<GameEntityType,
                static_cast<int32_t>(GameEntityType::Count)> targetTypes;
        };

        struct BatchTask
        {
            PtrNotNull<const GameSpatialSector> sector = nullptr;

            struct Value
            {
                GameSpatialMBR mbr;
                boost::container::small_vector<std::pair<PtrNotNull<Task>, Eigen::Vector2d>, 16> items;
            };

            std::array<std::optional<Value>, 4> quadPartition = {};
        };

    private:
        GameInstance& _gameInstance;
        std::chrono::milliseconds _scanInterval = std::chrono::milliseconds(200);

        bool _shutdown = false;
        bool _running = false;
        Future<void> _runFuture;
        boost::unordered::unordered_flat_map<game_entity_id_type, Task> _tasks;
        std::vector<boost::container::flat_set<game_entity_id_type>> _recycleBuffer;

        boost::unordered::unordered_flat_map<game_entity_id_type, std::function<void()>> _signals;
        boost::container::flat_map<game_spatial_sector_id_type, BatchTask> _batchTasks;
    };
}
