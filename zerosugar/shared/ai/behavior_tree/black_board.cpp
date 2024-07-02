#include "black_board.h"

namespace zerosugar::bt
{
    void BlackBoard::Clear()
    {
        _container.clear();
    }

    bool BlackBoard::Contains(const std::string& key) const
    {
        return _container.contains(key);
    }
}
