#include "sequence.h"

namespace zerosugar::bt::node
{
    bool Sequence::ShouldContinue(State childState) const
    {
        switch (childState)
        {
        case State::Success:
            return true;
        case State::Failure:
            return false;
        case State::Running:
            assert(false);
        }

        return false;
    }

    void from_xml(Sequence&, const pugi::xml_node&)
    {
    }
}
