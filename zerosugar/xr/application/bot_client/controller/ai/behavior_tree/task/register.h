#pragma once

namespace zerosugar::bt
{
    class NodeSerializer;
}

namespace zerosugar::xr::bot
{
    void Register(bt::NodeSerializer& serializer);
}
