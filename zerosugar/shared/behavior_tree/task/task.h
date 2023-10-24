#pragma once
#include <concepts>
#include <memory>
#include <string>
#include <optional>
#include <pugixml.hpp>
#include "zerosugar/shared/behavior_tree/task/runnable.h"

namespace zerosugar::bt
{
    class TaskFactory;

    class Task
    {
    public:
        Task(const Task& other) = delete;
        Task(Task&& other) noexcept = delete;
        Task& operator=(const Task& other) = delete;
        Task& operator=(Task&& other) noexcept = delete;

        Task() = default;
        virtual ~Task() = default;

        virtual void Initialize(const TaskFactory& factory, const pugi::xml_node& node) = 0;

        bool IsDone() const;

        auto Execute() -> State;
        virtual void Reset();

        auto GetState() const -> State;
        virtual auto GetName() const -> std::string_view = 0;

    private:
        virtual auto Run() const -> Runnable = 0;

    private:
        State _state = State::None;
        std::optional<Runnable> _runnable = std::nullopt;
    };

    template <typename T>
    concept task_concept = requires (T t)
    {
        requires std::derived_from<T, Task>;
        { T::class_name } -> std::convertible_to<std::string>;
    };

    template <typename T>
    class TaskInheritanceHelper : public Task
    {
    public:
        auto GetName() const -> std::string_view  override { return T::class_name; }
    };

    using task_pointer_type = std::unique_ptr<Task>;
}
