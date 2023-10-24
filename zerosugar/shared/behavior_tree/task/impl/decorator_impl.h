#pragma once
#include "zerosugar/shared/behavior_tree/task/decorator.h"

namespace zerosugar::bt
{
    class Repeat : public DecoratorInheritanceHelper<Repeat>
    {
    public:
        static constexpr const char* class_name = "repeat";

    public:
        void Initialize(const TaskFactory& factory, const pugi::xml_node& node) override;

    private:
        auto Run() const -> Runnable override;

    private:
        size_t _count = 0;
    };

    class RetryUntilSuccess : public DecoratorInheritanceHelper<RetryUntilSuccess>
    {
    public:
        static constexpr const char* class_name = "retry_until_success";

    public:
        void Initialize(const TaskFactory& factory, const pugi::xml_node& node) override;

    private:
        auto Run() const -> Runnable override;

    private:
        size_t _count = 0;
    };

    class Inverter : public DecoratorInheritanceHelper<Inverter>
    {
    public:
        static constexpr const char* class_name = "inverter";

    public:
        void Initialize(const TaskFactory& factory, const pugi::xml_node& node) override;

    private:
        auto Run() const -> Runnable override;
    };

    class ForceSuccess : public DecoratorInheritanceHelper<ForceSuccess>
    {
    public:
        static constexpr const char* class_name = "force_success";

    public:
        void Initialize(const TaskFactory& factory, const pugi::xml_node& node) override;

    private:
        auto Run() const -> Runnable override;
    };

    class ForceFailure : public DecoratorInheritanceHelper<ForceFailure>
    {
    public:
        static constexpr const char* class_name = "force_failure";

    public:
        void Initialize(const TaskFactory& factory, const pugi::xml_node& node) override;

    private:
        auto Run() const -> Runnable override;
    };
}
