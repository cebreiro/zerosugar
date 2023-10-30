#pragma once
#include <cassert>
#include <concepts>
#include <memory>
#include <string>
#include <optional>
#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/task/runnable.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class Task
    {
    public:
        Task(const Task& other) = delete;
        Task(Task&& other) noexcept = delete;
        Task& operator=(const Task& other) = delete;
        Task& operator=(Task&& other) noexcept = delete;

        Task() = delete;
        explicit Task(TContext& context);
        virtual ~Task() = default;

        virtual bool Initialize(const pugi::xml_node& node) = 0;

        auto Execute() -> State;
        virtual void Reset();

        auto GetState() const -> State;
        virtual auto GetName() const -> std::string_view = 0;

    private:
        virtual auto Run() const -> Runnable = 0;

    protected:
        std::remove_reference_t<TContext>& _context;

    private:
        State _state = State::None;
        std::optional<Runnable> _runnable = std::nullopt;
    };

    template <typename TContext, typename TModel>
    class TaskT : public Task<TContext>, public TModel
    {
    public:
        explicit TaskT(TContext& context);

        bool Initialize(const pugi::xml_node& node) override;

        auto GetName() const->std::string_view  override;
    };

    template <typename TContext>
    Task<TContext>::Task(TContext& context)
        : _context(context)
    {
    }

    template <typename TContext>
    auto Task<TContext>::Execute() -> State
    {
        if (!_runnable.has_value())
        {
            _runnable = this->Run();
            _state = _runnable->GetState();
        }

        if (_runnable->IsDone())
        {
            _state = _runnable->GetState();
        }
        else
        {
            _state = _runnable->Execute();
        }

        assert(_state != State::None);

        if (_state != State::Running)
        {
            _runnable.reset();
        }

        return _state;
    }

    template <typename TContext>
    void Task<TContext>::Reset()
    {
        _state = State::None;
        _runnable.reset();
    }

    template <typename TContext>
    auto Task<TContext>::GetState() const -> State
    {
        return _state;
    }

    template <typename TContext, typename TModel>
    TaskT<TContext, TModel>::TaskT(TContext& context)
        : Task<TContext>(context)
    {
    }

    template <typename TContext, typename TModel>
    bool TaskT<TContext, TModel>::Initialize(const pugi::xml_node& node)
    {
        return TModel::Deserialize(node);
    }

    template <typename TContext, typename TModel>
    auto TaskT<TContext, TModel>::GetName() const -> std::string_view
    {
        return TModel::class_name;
    }

    template <typename T, typename TContext>
    concept task_concept = requires (T t)
    {
        requires std::is_base_of_v<Task<TContext>, T>;
        { T::class_name } -> std::convertible_to<std::string>;
    };

    template <typename TContext>
    using task_pointer_type = std::unique_ptr<Task<TContext>>;
}
