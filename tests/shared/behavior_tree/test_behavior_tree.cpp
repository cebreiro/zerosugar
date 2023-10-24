#include "zerosugar/shared/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/behavior_tree/task/task.h"
#include "zerosugar/shared/behavior_tree/task/task_factory.h"


struct TestContext
{
    std::unordered_map<std::string, size_t> executes;
};

struct TestUnitTask : public zerosugar::bt::TaskInheritanceHelper<TestUnitTask>
{
    static constexpr const char* class_name = "test_unit_task";

    explicit TestUnitTask(TestContext& context)
        : _context(context)
    {
    }

    void Initialize(const zerosugar::bt::TaskFactory& factory, const pugi::xml_node& node) override
    {
        (void)factory;
        (void)node;
    }

private:
    auto Run() const -> zerosugar::bt::Runnable override
    {
        ++_context.executes[class_name];
        return zerosugar::bt::Runnable(zerosugar::bt::State::Success);
    }

private:
    TestContext& _context;
};
struct TestUnitRunningTask : public zerosugar::bt::TaskInheritanceHelper<TestUnitRunningTask>
{
    static constexpr const char* class_name = "test_unit_running_task";

    explicit TestUnitRunningTask(TestContext& context)
        : _context(context)
    {
        static_assert(zerosugar::bt::task_concept<TestUnitTask>, "");
    }

    void Initialize(const zerosugar::bt::TaskFactory& factory, const pugi::xml_node& node) override
    {
        (void)factory;
        (void)node;
    }

private:
    auto Run() const -> zerosugar::bt::Runnable override
    {
        ++_context.executes[class_name];

        co_await zerosugar::bt::running;
        co_return true;
    }

private:
    TestContext& _context;
};

class TestUnitTaskFactory : public zerosugar::bt::TaskFactory
{
public:
    explicit TestUnitTaskFactory(TestContext& context)
        : _context(context)
    {
        Register<TestUnitTask>();
        Register<TestUnitRunningTask>();
    }

    auto CreateTask(const std::string& name) const -> zerosugar::bt::task_pointer_type override
    {
        auto iter = _factories.find(name);
        if (iter != _factories.end())
        {
            return iter->second(_context);
        }

        return TaskFactory::CreateTask(name);
    }

    template <typename T>
    void Register()
    {
        [[maybe_unused]]
        const bool result = _factories.try_emplace(T::class_name, [](TestContext& context)
            {
                return std::make_unique<T>(context);
            }).second;
        assert(result);
    }

private:
    std::reference_wrapper<TestContext> _context;
    std::unordered_map<std::string, std::function<zerosugar::bt::task_pointer_type(TestContext&)>> _factories;
};

TEST(BehaviorTree, TestSequence)
{

static const std::string test_xml = R""""(
 <root>
    <sequence>
        <test_unit_task />
        <test_unit_running_task />
    </sequence>
 </root>
)"""";

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(test_xml.data(), test_xml.size());
    if (!result)
    {
        ASSERT_TRUE(false);
    }

    // arrange
    const pugi::xml_node& child = doc.child("root");
    ASSERT_TRUE(child);

    TestContext context;
    zerosugar::BehaviorTree behaviorTree;

    TestUnitTaskFactory factory(context);
    behaviorTree.Initialize(factory, doc.child("root"));

    // act
    while (behaviorTree.Execute() == zerosugar::bt::State::Running)
    {
    }

    // assert
    EXPECT_EQ(behaviorTree.GetState(), zerosugar::bt::State::Success);
    EXPECT_EQ(context.executes[TestUnitTask::class_name], 1);
    EXPECT_EQ(context.executes[TestUnitRunningTask::class_name], 1);
}
