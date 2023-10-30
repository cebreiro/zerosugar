#include "task.proto.h"

#include <cassert>
#include <cstring>

namespace zerosugar::bt::model
{
    bool Sequence::Deserialize(const pugi::xml_node& node)
    {
        assert(!::_stricmp(node.name(), class_name));

        if (!Model::Deserialize(node))
        {
            return false;
        }

        for (const pugi::xml_node& child : node.children())
        {
            auto model = ModelFactory::GetInstance().CreateModel(child);
            if (!model)
            {
                return false;
            }
            _children.emplace_back(std::move(model));
        }

        return true;
    }

    ModelFactory::Registry<Sequence> Sequence::_registry;

    bool Selector::Deserialize(const pugi::xml_node& node)
    {
        assert(!::_stricmp(node.name(), class_name));

        if (!Model::Deserialize(node))
        {
            return false;
        }

        for (const pugi::xml_node& child : node.children())
        {
            auto model = ModelFactory::GetInstance().CreateModel(child);
            if (!model)
            {
                return false;
            }
            _children.emplace_back(std::move(model));
        }

        return true;
    }

    ModelFactory::Registry<Selector> Selector::_registry;

    bool ForceSuccess::Deserialize(const pugi::xml_node& node)
    {
        assert(!::_stricmp(node.name(), class_name));

        if (!Model::Deserialize(node))
        {
            return false;
        }

        _child = ModelFactory::GetInstance().CreateModel(node.first_child());
        if (!_child)
        {
            return false;
        }

        return true;
    }

    ModelFactory::Registry<ForceSuccess> ForceSuccess::_registry;

    bool ForceFailure::Deserialize(const pugi::xml_node& node)
    {
        assert(!::_stricmp(node.name(), class_name));

        if (!Model::Deserialize(node))
        {
            return false;
        }

        _child = ModelFactory::GetInstance().CreateModel(node.first_child());
        if (!_child)
        {
            return false;
        }

        return true;
    }

    ModelFactory::Registry<ForceFailure> ForceFailure::_registry;

    bool Inverter::Deserialize(const pugi::xml_node& node)
    {
        assert(!::_stricmp(node.name(), class_name));

        if (!Model::Deserialize(node))
        {
            return false;
        }

        _child = ModelFactory::GetInstance().CreateModel(node.first_child());
        if (!_child)
        {
            return false;
        }

        return true;
    }

    ModelFactory::Registry<Inverter> Inverter::_registry;

    bool Repeat::Deserialize(const pugi::xml_node& node)
    {
        assert(!::_stricmp(node.name(), class_name));

        if (!Model::Deserialize(node))
        {
            return false;
        }

        _child = ModelFactory::GetInstance().CreateModel(node.first_child());
        if (!_child)
        {
            return false;
        }

        if (auto opt = Param::As<int32_t>(node.attribute("count")); opt.has_value())
        {
            _count = std::move(*opt);
        }
        else
        {
            return false;
        }

        return true;
    }

    ModelFactory::Registry<Repeat> Repeat::_registry;

    bool RetryUntilSuccess::Deserialize(const pugi::xml_node& node)
    {
        assert(!::_stricmp(node.name(), class_name));

        if (!Model::Deserialize(node))
        {
            return false;
        }

        _child = ModelFactory::GetInstance().CreateModel(node.first_child());
        if (!_child)
        {
            return false;
        }

        if (auto opt = Param::As<int32_t>(node.attribute("count")); opt.has_value())
        {
            _count = std::move(*opt);
        }
        else
        {
            return false;
        }

        return true;
    }

    ModelFactory::Registry<RetryUntilSuccess> RetryUntilSuccess::_registry;

}
