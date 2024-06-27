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
        template <typename TRange>
        void SetTargetIds(TRange&& range)
        {
            for (const game_entity_id_type id : range)
            {
                _targetIds.push_back(id.Unwrap());
            }
        }

    private:
        virtual bool SelectTarget(const GameInstance& gameInstance) = 0;
        virtual void StartExecution(GameInstance& gameInstance) = 0;
        virtual void OnComplete(GameInstance& gameInstance) = 0;

    private:
        std::chrono::system_clock::time_point _creationTimePoint;
        std::chrono::system_clock::time_point _baseTimePoint;
        boost::container::small_vector<int64_t, 8> _targetIds;

        static thread_local GameTask* _localInstance;
    };

    template <typename TBase, typename TParam>
    concept game_task_param_concept = requires (TBase& base)
    {
        requires std::derived_from<TParam, TBase>;
        { base.template Cast<TParam>() } -> std::same_as<const TParam*>;
    };

    template <typename TBase, typename TParam, game_task_target_selector_concept... TSelector>
         requires game_task_param_concept<TBase, TParam>
    class GameTaskT : public GameTask
    {
        static_assert(sizeof...(TSelector) > 0);
        static constexpr auto sequence = std::make_index_sequence<sizeof...(TSelector)>();

        static constexpr int64_t selector_count = sizeof...(TSelector);

    public:
        GameTaskT(std::chrono::system_clock::time_point creationTimePoint, UniquePtrNotNull<TBase> param, TSelector&&... selector)
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

            this->SetTargetIds(getTargetIds(_tuple, sequence) | std::views::join);

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

        void StartExecution(GameInstance& gameInstance) final
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
            const TParam* param = _param->template Cast<TParam>();
            assert(param);

            return *param;
        }

    private:
        UniquePtrNotNull<TBase> _param;
        std::tuple<TSelector...> _tuple;
    };
}
