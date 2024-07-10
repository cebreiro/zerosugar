#pragma once
#include <boost/container/small_vector.hpp>
#include "zerosugar/xr/server/game/instance/game_type.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/task/game_task_target_selector.h"

namespace zerosugar::xr
{
    class GameExecutionParallel;
    class GameExecutionSerial;
}

namespace zerosugar::xr
{
    class Entity;
    class GameInstance;

    class GameTask
    {
    public:
        GameTask() = delete;
        explicit GameTask(game_time_point_type creationTimePoint);

        virtual ~GameTask();

        virtual bool SelectTargetIds(const GameExecutionSerial& serialContext) = 0;

        virtual bool ShouldPrepareBeforeScheduled() const;
        virtual void Prepare(GameExecutionSerial& serialContext, bool& quickExit);
        virtual void OnFailTargetSelect(GameExecutionSerial& serialContext);

        void Start(GameExecutionParallel& parallelContext);
        void Complete(GameExecutionSerial& serialContext);

        auto GetCreationTimePoint() const -> game_time_point_type;
        auto GetBaseTime() const -> game_time_point_type;
        auto GetTargetIds() const -> const boost::container::small_vector<int64_t, 8>&;

        static bool IsInExecution();
        static auto GetLocalInstance() -> GameTask&;

    protected:
        template <typename TRange>
        void SetTargetIds(TRange&& range)
        {
            for (const game_entity_id_type id : range)
            {
                _targetIds.push_back(id.Unwrap());
            }
        }

    private:
        virtual bool SelectTarget(const GameExecutionParallel& parallelContext) = 0;
        virtual void StartExecution(GameExecutionParallel& parallelContext) = 0;
        virtual void OnComplete(GameExecutionSerial& serialContext) = 0;

    private:
        game_time_point_type _creationTimePoint;
        game_time_point_type _baseTimePoint;
        boost::container::small_vector<int64_t, 8> _targetIds;

        static thread_local GameTask* _localInstance;
    };

    template <typename TBase, typename TParam>
    concept game_task_param_concept = requires (TBase& base)
    {
        requires std::derived_from<TParam, TBase>;
        { base.template Cast<TParam>() } -> std::same_as<const TParam*>;
    };

    template <game_task_target_selector_concept... TSelector>
    class GameTaskT : public GameTask
    {
        static_assert(sizeof...(TSelector) > 0);
        static constexpr auto sequence = std::make_index_sequence<sizeof...(TSelector)>();

        static constexpr int64_t selector_count = sizeof...(TSelector);

    public:
        GameTaskT(game_time_point_type creationTimePoint, TSelector&&... selector)
            : GameTask(creationTimePoint)
            , _tuple({ std::forward<TSelector>(selector)... })
        {
        }

        bool SelectTargetIds(const GameExecutionSerial& serialContext) final
        {
            const auto select = [&serialContext]<typename T, size_t... I>(T && tuple, std::index_sequence<I...>)
            {
                return (... & std::get<I>(tuple).SelectEntityId(serialContext));
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

            this->SetTargetIds(getTargetIds(_tuple, sequence) | std::views::join);

            return true;
        }

        bool SelectTarget(const GameExecutionParallel& parallelContext) final
        {
            const auto select = [&parallelContext]<typename T, size_t... I>(T && tuple, std::index_sequence<I...>)
            {
                return (... & std::get<I>(tuple).SelectEntity(parallelContext));
            };

            const bool success = select(_tuple, sequence);

            return success;
        }

        void StartExecution(GameExecutionParallel& parallelContext) final
        {
            constexpr auto getTarget = []<typename T, size_t... I>(T&& tuple, std::index_sequence<I...>)
            {
                return std::tuple{ std::get<I>(tuple).GetTarget()... };
            };

            std::apply(std::bind_front(&GameTaskT::Execute, this, std::ref(parallelContext)), getTarget(_tuple, sequence));
        }

    protected:
        template <typename T> requires std::disjunction_v<std::is_same<T, TSelector>...>
        auto GetSelector() const -> const T&
        {
            return std::get<T>(_tuple);
        }

    private:
        virtual void Execute(GameExecutionParallel& parallelContext, typename TSelector::target_type...) = 0;

    private:
        std::tuple<TSelector...> _tuple;
    };

    template <typename TParam, game_task_target_selector_concept... TSelector>
    class GameTaskParamT : public GameTaskT<TSelector...>
    {
    public:
        GameTaskParamT(game_time_point_type creationTimePoint, TParam param, TSelector&&... selector)
            : GameTaskT<TSelector...>(creationTimePoint, std::forward<TSelector>(selector)...)
            , _param(std::move(param))
        {
        }

    protected:
        auto GetParam() const -> const TParam&
        {
            return _param;
        }

        auto MutableParam() -> TParam&
        {
            return _param;
        }

    private:
        TParam _param;
    };

    template <typename TBase, typename TParam, game_task_target_selector_concept... TSelector>
        requires game_task_param_concept<TBase, TParam>
    class GameTaskBaseParamT : public GameTaskT<TSelector...>
    {
    public:
        GameTaskBaseParamT(game_time_point_type creationTimePoint, UniquePtrNotNull<TBase> param, TSelector&&... selector)
            : GameTaskT<TSelector...>(creationTimePoint, std::forward<TSelector>(selector)...)
            , _param(std::move(param))
        {
        }

    protected:
        auto GetParam() const -> const TParam&
        {
            const TParam* param = _param->template Cast<TParam>();
            assert(param);

            return *param;
        }

    private:
        UniquePtrNotNull<TBase> _param;
    };
}
