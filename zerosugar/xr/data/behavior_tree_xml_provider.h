#pragma once
#include <pugixml.hpp>

namespace zerosugar::bt
{
    class INodeDataSet;
}

namespace zerosugar::xr
{
    class BehaviorTreeXMLProvider : public IService
    {
    public:
        ~BehaviorTreeXMLProvider();

        void Initialize(ServiceLocator& serviceLocator) override;
        void StartUp(const std::filesystem::path& directory);

        bool Add(const std::string& key, pugi::xml_document document);

        auto Find(const std::string& key) const -> const bt::INodeDataSet*;

        auto GetName() const->std::string_view override;

    private:
        ServiceLocatorT<ILogService> _serviceLocator;
        std::unordered_map<std::string, std::pair<pugi::xml_document, UniquePtrNotNull<bt::INodeDataSet>>> _dataSets;
    };
}
