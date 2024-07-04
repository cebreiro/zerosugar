#include "select_attack_target.h"

namespace zerosugar::xr::game
{
    auto SelectAttackTarget::Run() -> bt::node::Result
    {
        struct Foo{};

        co_await bt::Event<Foo>();

        co_return true;
    }

    auto SelectAttackTarget::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(SelectAttackTarget&, const pugi::xml_node&)
    {
    }
}
