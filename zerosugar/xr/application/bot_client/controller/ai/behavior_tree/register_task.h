#pragma once

namespace zerosugar::bt
{
    class NodeSerializer;
}

namespace zerosugar::xr::bot
{
    void RegisterTask(bt::NodeSerializer& serializer);
}
