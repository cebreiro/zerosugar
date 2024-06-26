#pragma once
#include <boost/container/small_vector.hpp>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/task/game_task_target_selector.h"

namespace zerosugar::xr
{
    class Entity;
    class GameInstance;

    class GameTask
    {
    public:
        GameTask() = delete;
        explicit GameTask(std::chrono::system_clock::time_point creationTimePoint);

        virtual ~GameTask();

        virtual bool SelectTargetIds(const GameInstance& gameInstance) = 0;

        void Start(GameInstance& gameInstance);
        void Complete(GameInstance& gameInstance);

        auto GetCreationTimePoint() const -> std::chrono::system_clock::time_point;
        auto GetBaseTime() const -> std::chrono::system_clock::time_point;
        auto GetTargetIds() const -> const boost::container::small_vector<int64_t, 8>&;

        static bool IsInExecution();
        static auto GetLocalInstance() -> GameTask&;

    protected:
        template <typename TRange> requires std::same_as<std::ranges::range_value_t<TRange>, const game_entity_id_type>
        void SetTargetIds(TRange&& range)
        {
            std::ranges::copy(std::forward<TRange>(range) | std::transform([](const game_entity_id_type id)
                {
                    return id.Unwrap();
                }), std::back_inserter(_targetIds));
        }

    private:
        virtual bool SelectTarget(const GameInstance& gameInstance) = 0;
        virtual void Execute(GameInstance& gameInstance) = 0;
        virtual void OnComplete(GameInstance& gameInstance) = 0;

    private:
        std::chrono::system_clock::time_point _creationTimePoint;
        std::chrono::system_clock::time_point _baseTimePoint;
        boost::container::small_vector<int64_t, 8> _targetIds;

        static thread_local GameTask* _localInstance;
    };

    template <typename TParam, game_task_target_selector_concept... TSelector>
    class GameTaskT : public GameTask
    {
        static_assert(sizeof...(TSelector) > 0);
        static constexpr auto sequence = std::make_index_sequence<sizeof...(TSelector)>();

        static constexpr int64_t selector_count = sizeof...(TSelector);

    public:
        GameTaskT(std::chrono::system_clock::time_point creationTimePoint, TParam param, TSelector&&... selector)
            : GameTask(creationTimePoint)
            , _param(std::move(param))
            , _tuple({ std::forward<TSelector>(selector)... })
        {
        }

        bool SelectTargetIds(const GameInstance& gameInstance) final
        {
            const auto select = [&gameInstance]<typename T, size_t... I>(T && tuple, std::index_sequence<I...>)
            {
                return (... & std::get<I>(tuple).SelectEntityId(gameInstance));
            };

            const bool success = select(_tuple, sequence);
            if (!success)
            {
                return false;
            }

            constexpr auto getTargetIds = []<typename T, size_t... I>(T&& tuple, std::index_sequence<I...>)
            {
                return std::array<std::span<const game_entity_id_type>, selector_count>{ std::get<I>(tuple).GetTargetId()... };
            };

            const auto array = getTargetIds(_tuple, sequence);
            auto range = array | std::views::join;

            this->SetTargetIds(std::move(range));

            return true;
        }

        bool SelectTarget(const GameInstance& gameInstance) final
        {
            const auto select = [&gameInstance]<typename T, size_t... I>(T && tuple, std::index_sequence<I...>)
            {
                return (... & std::get<I>(tuple).SelectEntity(gameInstance));
            };

            const bool success = select(_tuple, sequence);

            return success;
        }

        void Execute(GameInstance& gameInstance) final
        {
            constexpr auto getTarget = []<typename T, size_t... I>(T&& tuple, std::index_sequence<I...>)
            {
                return std::tuple{ std::get<I>(tuple).GetTarget()... };
            };

            std::apply(std::bind_front(&GameTaskT::Execute, this, std::ref(gameInstance)), getTarget(_tuple, sequence));
        }

        virtual void Execute(GameInstance& gameInstance, typename TSelector::target_type...) = 0;

    protected:
        auto GetParam() const -> const TParam&
        {
            return _param;
        }

    private:
        TParam _param;
        std::tuple<TSelector...> _tuple;
    };
}
