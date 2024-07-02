#pragma once
#include "zerosugar/shared/type/not_null_pointer.h"

namespace pugi
{
    class xml_node;
}

namespace zerosugar::bt
{
    class INode;
}

namespace zerosugar::bt
{
    class INodeSerializer
    {
    public:
        virtual ~INodeSerializer() = default;

        virtual auto Deserialize(const pugi::xml_node& xmlNode) const -> UniquePtrNotNull<INode> = 0;
    };
}
