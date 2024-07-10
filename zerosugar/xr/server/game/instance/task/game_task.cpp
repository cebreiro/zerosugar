#include "game_task.h"

#include <boost/scope/scope_exit.hpp>

namespace zerosugar::xr
{
    thread_local GameTask* GameTask::_localInstance = nullptr;

    GameTask::GameTask(game_time_point_type creationTimePoint)
        : _creationTimePoint(creationTimePoint)
    {
    }

    GameTask::~GameTask()
    {
    }

    bool GameTask::ShouldPrepareBeforeScheduled() const
    {
        return false;
    }

    void GameTask::Prepare(GameExecutionSerial& serialContext, bool& quickExit)
    {
        (void)serialContext;
        (void)quickExit;
    }

    void GameTask::OnFailTargetSelect(GameExecutionSerial& serialContext)
    {
        (void)serialContext;
    }

    void GameTask::Start(GameExecutionParallel& parallelContext)
    {
        if (!this->SelectTarget(parallelContext))
        {
            return;
        }

        _localInstance = this;

        boost::scope::scope_exit exit([this]()
            {
                assert(_localInstance == this);

                _localInstance = nullptr;
            });

        _baseTimePoint = game_clock_type::now();

        this->StartExecution(parallelContext);
    }

    void GameTask::Complete(GameExecutionSerial& serialContext)
    {
        this->OnComplete(serialContext);
    }

    auto GameTask::GetCreationTimePoint() const -> game_time_point_type
    {
        return _creationTimePoint;
    }

    auto GameTask::GetTargetIds() const -> const boost::container::small_vector<int64_t, 8>&
    {
        return _targetIds;
    }

    bool GameTask::IsInExecution()
    {
        return _localInstance != nullptr;
    }

    auto GameTask::GetLocalInstance() -> GameTask&
    {
        assert(_localInstance);

        return *_localInstance;
    }

    auto GameTask::GetBaseTime() const -> game_time_point_type
    {
        return _baseTimePoint;
    }
}
