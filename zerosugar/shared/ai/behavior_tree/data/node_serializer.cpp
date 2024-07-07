#include "node_serializer.h"

#include <stdexcept>
#include <format>
#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/node/branch/selector.h"
#include "zerosugar/shared/ai/behavior_tree/node/branch/sequence.h"
#include "zerosugar/shared/ai/behavior_tree/node/decorator/inverter.h"
#include "zerosugar/shared/ai/behavior_tree/node/decorator/force_success.h"
#include "zerosugar/shared/ai/behavior_tree/node/decorator/force_failure.h"
#include "zerosugar/shared/ai/behavior_tree/node/task/suspend_forever.h"

namespace zerosugar::bt
{
    NodeSerializer::NodeSerializer()
    {
        RegisterXML<node::Selector>();
        RegisterXML<node::Sequence>();

        RegisterXML<node::Inverter>();
        RegisterXML<node::ForceSuccess>();
        RegisterXML<node::ForceFailure>();

        RegisterXML<node::SuspendForever>();
    }

    auto NodeSerializer::Deserialize(const pugi::xml_node& xmlNode) const -> UniquePtrNotNull<INode>
    {
        const std::string& name = xmlNode.name();

        auto iter = _xml.find(name);
        if (iter == _xml.end())
        {
            assert(false);

            throw std::runtime_error(std::format(
                "[bt node deserializer] fail to find node adl function. node name: {}", name));
        }

        return iter->second(xmlNode);
    }
}
