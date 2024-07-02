#include "inverter.h"

namespace zerosugar::bt::node
{
    auto Inverter::Decorate(State state) const -> State
    {
        assert(state != State::Running);

        return state == State::Success ? State::Failure : State::Success;
    }

    void from_xml(Inverter&, const pugi::xml_node&)
    {
    }
}
