#pragma once
#include <vector>
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar::bt
{
    class INode;
    class NodeSerializer;
}

namespace zerosugar::bt
{
    class INodeDataSet
    {
    public:
        virtual ~INodeDataSet() = default;

        virtual auto Deserialize(const NodeSerializer& serializer) const -> std::vector<UniquePtrNotNull<INode>> = 0;
    };
}
