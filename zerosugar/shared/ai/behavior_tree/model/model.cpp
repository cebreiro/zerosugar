#include "model.h"

namespace zerosugar::bt::model
{
    bool Model::Deserialize(const pugi::xml_node& node)
    {
        if (!node)
        {
            return false;
        }

        _name = node.name();

        if (_name.empty())
        {
            return false;
        }

        return true;
    }

    auto Model::GetName() const -> const std::string&
    {
        return _name;
    }

    bool Param::Deserialize(const pugi::xml_attribute& attribute)
    {
        if (attribute.empty())
        {
            return false;
        }

        _name = attribute.name();
        _rawString = attribute.as_string();

        if (_name.empty())
        {
            return false;
        }

        return true;
    }

    auto Param::GetName() const -> const std::string&
    {
        return _name;
    }

    auto Param::GetRawString() const -> const std::string&
    {
        return _rawString;
    }

    auto ModelFactory::GetInstance() -> ModelFactory&
    {
        static ModelFactory instance;
        return instance;
    }

    auto ModelFactory::CreateModel(const pugi::xml_node& node) -> std::unique_ptr<Model>
    {
        if (!node)
        {
            return nullptr;
        }

        auto iter = _table.find(node.name());
        if (iter == _table.end())
        {
            return nullptr;
        }

        auto model = iter->second();
        if (!model->Deserialize(node))
        {
            return nullptr;
        }

        return model;
    }
}
