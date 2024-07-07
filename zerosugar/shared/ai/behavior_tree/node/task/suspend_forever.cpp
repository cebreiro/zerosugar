#include "suspend_forever.h"

namespace zerosugar::bt::node
{
    auto SuspendForever::Run() -> Result
    {
        struct SuspendForever{};

        co_await bt::Event<SuspendForever>();

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
