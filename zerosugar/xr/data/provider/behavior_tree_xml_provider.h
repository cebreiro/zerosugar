#pragma once
#include <pugixml.hpp>

namespace zerosugar::bt
{
    class INodeDataSet;
}

namespace zerosugar::xr
{
    class BehaviorTreeXMLProvider
    {
    public:
        ~BehaviorTreeXMLProvider();

        void Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath);
        void StartUp(const std::filesystem::path& basePath);

        auto Find(const std::string& key) const -> const bt::INodeDataSet*;

        auto GetName() const->std::string_view;

    private:
        auto ReadXMLFrom(ServiceLocator& serviceLocator, const std::filesystem::path& directory) -> std::vector<std::tuple<std::string, pugi::xml_document, UniquePtrNotNull<bt::INodeDataSet>>>;

    private:
        std::unordered_map<std::string, std::pair<pugi::xml_document, UniquePtrNotNull<bt::INodeDataSet>>> _dataSets;
    };
}
