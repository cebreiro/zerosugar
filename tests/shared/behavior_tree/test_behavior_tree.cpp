#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/task/task.h"
#include "zerosugar/shared/ai/behavior_tree/task/task_factory.h"
#include "zerosugar/shared/ai/behavior_tree/model/model.h"


using zerosugar::bt::TaskT;
using zerosugar::bt::model::Model;
using zerosugar::bt::model::ModelFactory;

struct TestContext
{
    std::unordered_map<std::string, size_t> executes;
};

struct TestUnitTaskModel : Model
{
    static constexpr const char* class_name = "test_unit_task";

    bool Deserialize(const pugi::xml_node& node) override
    {
        coefficient = node.attribute("coefficient").as_int();

        return true;
    }

    int32_t coefficient = 0;

    static ModelFactory::Registry<TestUnitTaskModel> registry;
};

ModelFactory::Registry<TestUnitTaskModel> TestUnitTaskModel::registry;

struct TestUnitTask : TaskT<TestContext, TestUnitTaskModel>
{
    explicit TestUnitTask(TestContext& context)
        : TaskT(context)
    {
    }

private:
    auto Run() const -> zerosugar::bt::Runnable override
    {
        _context.executes[class_name] += this->coefficient;

        return zerosugar::bt::Runnable(zerosugar::bt::State::Success);
    }
};
template class zerosugar::bt::TaskFactory<TestContext>::Registry<TestUnitTask>;


struct TestUnitRunningTaskModel : Model
{
    static constexpr const char* class_name = "test_unit_running_task";

    bool Deserialize(const pugi::xml_node& node) override
    {
        coefficient = node.attribute("coefficient").as_int();

        return true;
    }

    int32_t coefficient = 0;

    static ModelFactory::Registry<TestUnitRunningTaskModel> registry;
};

ModelFactory::Registry<TestUnitRunningTaskModel> TestUnitRunningTaskModel::registry;

struct TestUnitRunningTask : public TaskT<TestContext, TestUnitRunningTaskModel>
{
    static constexpr const char* class_name = "test_unit_running_task";

    explicit TestUnitRunningTask(TestContext& context)
        : TaskT(context)
    {
    }

private:
    auto Run() const -> zerosugar::bt::Runnable override
    {
        _context.executes[class_name] += this->coefficient;

        co_await zerosugar::bt::running;
        co_return true;
    }
};
template class zerosugar::bt::TaskFactory<TestContext>::Registry<TestUnitRunningTask>;

TEST(BehaviorTree, TestSequence)
{
    static constexpr int32_t coefficient1 = 3;
    static constexpr int32_t coefficient2 = 5;

const std::string test_xml = std::format(R""""(
 <root>
    <sequence>
        <test_unit_task           coefficient="{}" />
        <test_unit_running_task   coefficient="{}" />
    </sequence>
 </root>
)"""", coefficient1, coefficient2);

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

    const bool init = behaviorTree.Initialize(doc.child("root"));
    ASSERT_TRUE(init);

    // act
    while (behaviorTree.Execute() == zerosugar::bt::State::Running)
    {
    }

    // assert

    EXPECT_EQ(behaviorTree.GetState(), zerosugar::bt::State::Success);
    EXPECT_EQ(context.executes[TestUnitTask::class_name], coefficient1);
    EXPECT_EQ(context.executes[TestUnitRunningTask::class_name], coefficient2);
}
