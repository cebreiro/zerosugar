#include "zerosugar/shared/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/behavior_tree/task/task.h"
#include "zerosugar/shared/behavior_tree/task/task_factory.h"


struct TestContext
{
    std::unordered_map<std::string, size_t> executes;
};

struct TestUnitTask : public zerosugar::bt::TaskInheritanceHelper<TestUnitTask, TestContext>
{
    static constexpr const char* class_name = "test_unit_task";

    explicit TestUnitTask(TestContext& context)
        : TaskInheritanceHelper(context)
    {
    }

    void Initialize(const pugi::xml_node& node) override
    {
        (void)node;
    }

private:
    auto Run() const -> zerosugar::bt::Runnable override
    {
        ++_context.executes[class_name];
        return zerosugar::bt::Runnable(zerosugar::bt::State::Success);
    }
};
template class zerosugar::bt::TaskFactory<TestContext>::Registry<TestUnitTask>;

struct TestUnitRunningTask : public zerosugar::bt::TaskInheritanceHelper<TestUnitRunningTask, TestContext>
{
    static constexpr const char* class_name = "test_unit_running_task";

    explicit TestUnitRunningTask(TestContext& context)
        : TaskInheritanceHelper(context)
    {
    }

    void Initialize(const pugi::xml_node& node) override
    {
        (void)node;
    }

private:
    auto Run() const -> zerosugar::bt::Runnable override
    {
        ++_context.executes[class_name];

        co_await zerosugar::bt::running;
        co_return true;
    }
};
template class zerosugar::bt::TaskFactory<TestContext>::Registry<TestUnitRunningTask>;

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
    zerosugar::BehaviorTree<TestContext> behaviorTree(context);
    behaviorTree.Initialize(doc.child("root"));

    // act
    while (behaviorTree.Execute() == zerosugar::bt::State::Running)
    {
    }

    // assert
    EXPECT_EQ(behaviorTree.GetState(), zerosugar::bt::State::Success);
    EXPECT_EQ(context.executes[TestUnitTask::class_name], 1);
    EXPECT_EQ(context.executes[TestUnitRunningTask::class_name], 1);
}
