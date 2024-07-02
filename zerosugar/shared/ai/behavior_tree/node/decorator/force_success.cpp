#include "force_success.h"

namespace zerosugar::bt::node
{
    auto ForceSuccess::Decorate(State state) const -> State
    {
        (void)state;

        return State::Success;
    }

    void from_xml(ForceSuccess&, const pugi::xml_node&)
    {
    }
}
