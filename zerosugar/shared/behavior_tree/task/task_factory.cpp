#include "task_factory.h"

#include "zerosugar/shared/behavior_tree/task/impl/task_impl.h"
#include "zerosugar/shared/behavior_tree/task/impl/decorator_impl.h"

namespace zerosugar::bt
{
    TaskFactory::Registry TaskFactory::_register;

    TaskFactory::Registry::Registry()
    {
        Register<Sequence>();
        Register<Selector>();

        Register<Repeat>();
        Register<RetryUntilSuccess>();
        Register<Inverter>();
        Register<ForceSuccess>();
        Register<ForceFailure>();
    }

    auto TaskFactory::CreateTask(const std::string& name) const -> task_pointer_type
    {
        auto iter = _register.factories.find(name);
        return iter != _register.factories.end() ? iter->second() : task_pointer_type{};
    }
}
