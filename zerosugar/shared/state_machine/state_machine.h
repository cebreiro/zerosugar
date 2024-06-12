#pragma once

namespace zerosugar
{
    template <typename TParam, typename TResult = void>
    struct StateEvent
    {
        using param_type = TParam;
        using result_type = TResult;
    };

    template <typename EState, typename TEvent> requires std::is_scoped_enum_v<EState>
    class StateMachine;

    template <typename EState, typename TEvent> requires std::is_scoped_enum_v<EState>
    class IState
    {
        using state_machine_type = StateMachine<EState, TEvent>;
        using event_param_type = typename TEvent::param_type;
        using event_result_type = typename TEvent::result_type;

    public:
        IState() = delete;
        IState(const IState&) = delete;
        IState& operator=(const IState&) = delete;

        explicit IState(EState state)
            : _state(state)
        {
        }

        virtual ~IState() = default;

        virtual void OnEnter() {}
        virtual void OnExit() {}

        virtual auto OnEvent(const event_param_type& event) -> event_result_type = 0;

        auto GetState() const -> EState
        {
            return _state;
        }

    private:
        EState _state;
    };

    template <typename EState, typename TEvent> requires std::is_scoped_enum_v<EState>
    class StateMachine
    {
    public:
        using state_type = IState<EState, TEvent>;
        using event_param_type = typename TEvent::param_type;
        using event_result_type = typename TEvent::result_type;

    public:
        class StateTransition
        {
        public:
            auto Add(EState state) -> StateTransition&
            {
                assert(std::ranges::find(_states, state) == _states.end());

                _states.push_back(state);

                return *this;
            }

            bool Has(EState state) const
            {
                auto iter = std::ranges::find(_states, state);
                return iter != _states.end();
            }

        private:
            std::vector<EState> _states;
        };

    public:
        StateMachine() = default;
        StateMachine(const StateMachine& other) = delete;
        StateMachine(StateMachine&& other) noexcept = delete;
        StateMachine& operator=(const StateMachine& other) = delete;
        StateMachine& operator=(StateMachine&& other) noexcept = delete;

        template <typename T, typename... Args>
        auto AddState(EState state, bool setCurrent, Args&&... args) -> StateTransition&
        {
            assert(!_transitions.contains(state));

            state_type* instance = _states.emplace_back(std::make_shared<T>(std::forward<Args>(args)...)).get();
            auto iter = _transitions.try_emplace(state, StateTransition{}).first;

            if (setCurrent)
            {
                _currentState = instance;
                _currentTransition = &iter->second;
            }

            return iter->second;
        }

        bool Transition(EState state)
        {
            assert(_currentTransition);

            if (!_currentTransition->Has(state))
            {
                return false;
            }

            auto [newState, transition] = Find(state);
            if (!newState)
            {
                return false;
            }

            _currentState->OnExit();

            _currentState = newState;
            _currentTransition = transition;

            _currentState->OnEnter();

            return true;
        }

        auto OnEvent(const event_param_type& event) -> event_result_type
        {
            assert(_currentState);

            return _currentState->OnEvent(event);
        }

        auto GetCurrentState() const -> EState
        {
            assert(_currentState);

            return _currentState->GetState();
        }

    private:
        auto Find(EState state) -> std::pair<state_type*, StateTransition*>
        {
            std::pair<state_type*, StateTransition*> result = {};

            const auto finder = [state](const SharedPtrNotNull<state_type>& instance)
                {
                    return instance->GetState() == state;
                };

            if (auto iter = std::ranges::find_if(_states, finder); iter != _states.end())
            {
                result.first = iter->get();
            }

            if (auto iter = _transitions.find(state); iter != _transitions.end())
            {
                result.second = &iter->second;
            }

            assert((static_cast<bool>(result.first) ^ static_cast<bool>(result.second)) == false);

            return result;
        }

    private:
        state_type* _currentState = nullptr;
        StateTransition* _currentTransition = nullptr;

        std::vector<SharedPtrNotNull<state_type>> _states;
        std::unordered_map<EState, StateTransition> _transitions;
    };
}
