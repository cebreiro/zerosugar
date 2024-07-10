#pragma once
#include "zerosugar/xr/server/game/instance/game_type.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_id.h"
#include "zerosugar/xr/server/game/repository/game_repository_interface.h"

namespace zerosugar::xr
{
    class GameInstance;
    class GameTask;
    class GameEntityContainer;
}

namespace zerosugar::xr
{
    class GameExecutionParallel
    {
    public:
        GameExecutionParallel() = delete;

        explicit GameExecutionParallel(GameInstance& gameInstance);
        ~GameExecutionParallel();

        auto Hold() -> std::shared_ptr<GameExecutionParallel>;

        void SummitTask(UniquePtrNotNull<GameTask> task, std::optional<game_controller_id_type> controllerId = std::nullopt);

        auto GetExecutor() const -> execution::IExecutor&;
        auto GetServiceLocator() const -> ServiceLocator&;

        auto GetEntityContainer() -> GameEntityContainer&;
        auto GetEntityContainer() const -> const GameEntityContainer&;

    public:
        static auto GetBaseTimePoint() -> game_time_point_type;

    private:
        GameInstance& _gameInstance;
    };
}
