#include "behavior_tree_xml_provider.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_xml.h"

namespace zerosugar::xr
{
    BehaviorTreeXMLProvider::~BehaviorTreeXMLProvider()
    {
    }

    void BehaviorTreeXMLProvider::Initialize(ServiceLocator& serviceLocator)
    {
        _serviceLocator = serviceLocator;
    }

    void BehaviorTreeXMLProvider::StartUp(const std::filesystem::path& directory)
    {
        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            const auto& path = entry.path();

            if (!path.has_extension() || path.extension().string() != ".xml")
            {
                continue;
            }

            try
            {
                std::ifstream ifstream(path, std::ios::in);
                if (!ifstream.is_open())
                {
                    throw std::runtime_error("fail to open file");
                }

                std::ostringstream iss;
                iss << ifstream.rdbuf();

                std::string data = iss.str();
                ifstream.close();

                pugi::xml_document doc;
                pugi::xml_parse_result result = doc.load_buffer(data.data(), data.size());
                if (!result)
                {
                    throw std::runtime_error("fail to load xml");
                }

                auto key = path.stem().generic_string();

                if (!Add(key, std::move(doc)))
                {
                    throw std::runtime_error("fail to insert");
                }
            }
            catch (const std::exception& e)
            {
                ZEROSUGAR_LOG_ERROR(_serviceLocator,
                    std::format("[{}] fail to add behavior tree data. exception: {}, path: {}",
                        GetName(), e.what(), path.generic_string()));
            }
        }
    }

    bool BehaviorTreeXMLProvider::Add(const std::string& key, pugi::xml_document document)
    {
        UniquePtrNotNull<bt::INodeDataSet> dataSet = std::make_unique<bt::NodeDataSetXML>(document.child("root").first_child());

        return _dataSets.try_emplace(key, std::make_pair(std::move(document), std::move(dataSet))).second;
    }

    auto BehaviorTreeXMLProvider::Find(const std::string& key) const -> const bt::INodeDataSet*
    {
        const auto iter = _dataSets.find(key);

        return iter != _dataSets.end() ? iter->second.second.get() : nullptr;
    }

    auto BehaviorTreeXMLProvider::GetName() const -> std::string_view
    {
        return "behavior_tree_xml_provider";
    }
}
