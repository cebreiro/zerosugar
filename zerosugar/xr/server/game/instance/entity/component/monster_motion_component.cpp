#include "monster_motion_component.h"

namespace zerosugar::xr
{
    bool MonsterMotionComponent::IsRunning(MotionType type, game_time_point_type current) const
    {
        if (_type != type)
        {
            return false;
        }

        if (current < _startTimePoint || current >= _endTimePoint)
        {
            return false;
        }

        return true;
    }

    auto MonsterMotionComponent::ChangeMotion(MotionType type, double duration, game_time_point_type current) -> int64_t
    {
        CancelAttackEffects();

        _type = type;
        _startTimePoint = current;
        _endTimePoint = current + std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::duration<double, std::chrono::seconds::period>(duration));

        return ++_counter;
    }

    void MonsterMotionComponent::AddAttackEffectCancelToken(std::stop_source token)
    {
        _attackEffectCancelTokens.push_back(std::move(token));
    }

    void MonsterMotionComponent::CancelAttackEffects()
    {
        for (std::stop_source& token : _attackEffectCancelTokens)
        {
            token.request_stop();
        }

        _attackEffectCancelTokens.clear();
    }

    auto MonsterMotionComponent::GetCurrentMotionType() const -> MotionType
    {
        return _type;
    }

    auto MonsterMotionComponent::GetCounter() const -> int64_t
    {
        return _counter;
    }

    auto MonsterMotionComponent::GetMotionId() const -> int32_t
    {
        return _motionId;
    }

    void MonsterMotionComponent::SetMotionId(int32_t motionId)
    {
        _motionId = motionId;
    }
}
