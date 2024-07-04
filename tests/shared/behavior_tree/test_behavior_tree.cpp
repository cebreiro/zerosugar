#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_xml.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"

using namespace zerosugar;
using namespace zerosugar::bt;
using namespace zerosugar::bt::node;

struct AddTask : Leaf
{
    static constexpr const char* name = "add";

    auto Run() -> Result override
    {
        (void)GetBlackBoard().Insert("value", int32_t{ 0 });

        int32_t* value = GetBlackBoard().GetIf<int32_t>("value");
        *value += _value;

        return true;
    }

    friend void from_xml(AddTask& task, const pugi::xml_node& node)
    {
        if (auto attr = node.attribute("value"); attr)
        {
            task._value = attr.as_int();
        }
    }

    int32_t _value = 0;
};

struct MultiplyTask : Leaf
{
    static constexpr const char* name = "multiply";

    auto Run() -> Result override
    {
        if (GetBlackBoard().Contains("value"))
        {
            int32_t* value = GetBlackBoard().GetIf<int32_t>("value");
            *value *= _value;

            return true;
        }

        return false;
    }

    friend void from_xml(MultiplyTask& task, const pugi::xml_node& node)
    {
        if (auto attr = node.attribute("value"); attr)
        {
            task._value = attr.as_int();
        }
    }

    int32_t _value = 0;
};

struct RunningTask : Leaf
{
    struct ExpectTrue
    {
        static constexpr const char* name = "my_param1";

        bool eventValue = false;
    };

    static constexpr const char* name = "running";

    auto Run() -> Result override
    {
        while (true)
        {
            std::variant<ExpectTrue> va = co_await Event<ExpectTrue>{};
            if (const ExpectTrue* eventParam = std::get_if<ExpectTrue>(&va); eventParam)
            {
                if (eventParam->eventValue)
                {
                    co_return true;
                }
            }
        }
    }

    friend void from_xml(RunningTask& task, const pugi::xml_node& node)
    {
        if (auto attr = node.attribute("value"); attr)
        {
            task._value = attr.as_int();
        }
    }

    int32_t _value = 0;
};

class BehaviorTreeTest : public ::testing::Test
{
public:
    BehaviorTreeTest()
    {
        _serializer.RegisterXML<AddTask>();
        _serializer.RegisterXML<MultiplyTask>();
        _serializer.RegisterXML<RunningTask>();
    }

protected:
    NodeSerializer _serializer;
};

TEST_F(BehaviorTreeTest, TestSequence)
{
    static constexpr int32_t value1 = 3;
    static constexpr int32_t value2 = 5;

    const std::string test_xml = std::format(R""""(
 <root>
    <sequence>
        <add    value="{}" />
        <add    value="{}" />
    </sequence>
 </root>
)"""", value1, value2);

    // arrange
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(test_xml.data(), test_xml.size());
    if (!result)
    {
        ASSERT_TRUE(false);
    }

    BlackBoard blackBoard;
    BehaviorTree behaviorTree(blackBoard);

    NodeDataSetXML dataSet(doc.child("root").first_child());
    behaviorTree.Initialize("test", dataSet.Deserialize(_serializer));

    // act
    behaviorTree.RunOnce();

    // assert
    EXPECT_FALSE(behaviorTree.IsAwaiting());
    EXPECT_TRUE(blackBoard.Contains("value"));
    EXPECT_TRUE(blackBoard.ContainsAs<int32_t>("value"));
    EXPECT_EQ(*blackBoard.GetIf<int32_t>("value"), value1 + value2);
}

TEST_F(BehaviorTreeTest, TestSequenceOrder)
{
    static constexpr int32_t value1 = 1;
    static constexpr int32_t value2 = 2;
    static constexpr int32_t value3 = 3;
    static constexpr int32_t value4 = 4;
    static constexpr int32_t value5 = 5;

    const std::string test_xml = std::format(R""""(
 <root>
    <sequence>
        <add    value="{}" />
        <sequence>
            <multiply   value="{}" />
            <add    value="{}" />
            <multiply   value="{}" />
        </sequence>
        <add    value="{}" />
    </sequence>
 </root>
)"""", value1, value2, value3, value4, value5);

    // arrange
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(test_xml.data(), test_xml.size());
    if (!result)
    {
        ASSERT_TRUE(false);
    }

    BlackBoard blackBoard;
    BehaviorTree behaviorTree(blackBoard);

    NodeDataSetXML dataSet(doc.child("root").first_child());
    behaviorTree.Initialize("test", dataSet.Deserialize(_serializer));

    // act
    behaviorTree.RunOnce();

    // assert
    EXPECT_FALSE(behaviorTree.IsAwaiting());
    EXPECT_TRUE(blackBoard.Contains("value"));
    EXPECT_TRUE(blackBoard.ContainsAs<int32_t>("value"));
    EXPECT_EQ(*blackBoard.GetIf<int32_t>("value"), (value1 * value2 + value3) * value4 + value5);
}

TEST_F(BehaviorTreeTest, TestRunning)
{
    static constexpr int32_t value1 = 1;
    static constexpr int32_t value2 = 2;
    static constexpr int32_t value3 = 3;
    static constexpr int32_t value4 = 4;
    static constexpr int32_t value5 = 5;

    const std::string test_xml = std::format(R""""(
 <root>
    <sequence>
        <running />
    </sequence>
 </root>
)"""", value1, value2, value3, value4, value5);

    // arrange
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(test_xml.data(), test_xml.size());
    if (!result)
    {
        ASSERT_TRUE(false);
    }

    BlackBoard blackBoard;
    BehaviorTree behaviorTree(blackBoard);

    NodeDataSetXML dataSet(doc.child("root").first_child());
    behaviorTree.Initialize("test", dataSet.Deserialize(_serializer));

    // act
    behaviorTree.RunOnce();
    ASSERT_TRUE(behaviorTree.IsAwaiting());

    RunningTask::ExpectTrue param;
    param.eventValue = false;

    behaviorTree.NotifyAndResume(param);
    bool running1 = behaviorTree.IsAwaiting();

    param.eventValue = true;
    behaviorTree.NotifyAndResume(param);

    bool running2 = behaviorTree.IsAwaiting();

    // assert
    EXPECT_TRUE(running1);
    EXPECT_FALSE(running2);
}
