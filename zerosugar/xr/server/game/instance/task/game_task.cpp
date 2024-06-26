#include "game_task.h"

#include <boost/scope/scope_exit.hpp>

namespace zerosugar::xr
{
    thread_local GameTask* GameTask::_localInstance = nullptr;

    GameTask::GameTask(std::chrono::system_clock::time_point creationTimePoint)
        : _creationTimePoint(creationTimePoint)
    {
    }

    GameTask::~GameTask()
    {
    }

    void GameTask::Start(GameInstance& gameInstance) 
    {
        if (!this->SelectTarget(gameInstance))
        {
            return;
        }

        _localInstance = this;

        boost::scope::scope_exit exit([this]()
            {
                assert(_localInstance == this);

                _localInstance = nullptr;
            });

        _baseTimePoint = std::chrono::system_clock::now();

        this->Execute(gameInstance);
    }

    void GameTask::Complete(GameInstance& gameInstance)
    {
        this->OnComplete(gameInstance);
    }

    auto GameTask::GetCreationTimePoint() const -> std::chrono::system_clock::time_point
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

    auto GameTask::GetBaseTime() const -> std::chrono::system_clock::time_point
    {
        return _baseTimePoint;
    }
}
