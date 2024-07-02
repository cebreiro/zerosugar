#include "force_failure.h"

namespace zerosugar::bt::node
{
    auto ForceFailure::Decorate(State state) const -> State
    {
        (void)state;

        return State::Failure;
    }

    void from_xml(ForceFailure&, const pugi::xml_node&)
    {
    }
}
