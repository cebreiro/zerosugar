#include "suspend_forever.h"

#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/event/suspend_event.h"

namespace zerosugar::xr::bot
{
    auto SuspendForever::Run() -> bt::node::Result
    {
        co_await bt::Event<event::SuspendForever>();

        co_return false;
    }

    auto SuspendForever::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(SuspendForever&, const pugi::xml_node&)
    {
    }
}
