#include "selector.h"

namespace zerosugar::bt::node
{
    bool Selector::ShouldContinue(State childState) const
    {
        switch (childState)
        {
        case State::Success:
            return false;
        case State::Failure:
            return true;
        case State::Running:
            assert(false);
        }

        return false;
    }

    void from_xml(Selector&, const pugi::xml_node&)
    {
    }
}
