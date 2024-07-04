#include "behavior_tree_xml_provider.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_xml.h"

namespace zerosugar::xr
{
    BehaviorTreeXMLProvider::~BehaviorTreeXMLProvider()
    {
    }

    void BehaviorTreeXMLProvider::Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath)
    {
        if (const auto directory = basePath / "behavior_tree"; exists(directory))
        {
            for (auto& [key, doc, dataSet] : ReadXMLFrom(serviceLocator, directory))
            {
                [[maybe_unused]]
                const bool inserted = _dataSets.try_emplace(key, std::make_pair(std::move(doc), std::move(dataSet))).second;
                assert(inserted);
            }
        }
        else
        {
            ZEROSUGAR_LOG_ERROR(serviceLocator,
                std::format("[{}] fail to find bot behavior tree directory. path: {}",
                    GetName(), directory.generic_string()));
        }
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

    auto BehaviorTreeXMLProvider::ReadXMLFrom(ServiceLocator& serviceLocator, const std::filesystem::path& directory)
        -> std::vector<std::tuple<std::string, pugi::xml_document,UniquePtrNotNull<bt::INodeDataSet>>>
    {
        std::vector<std::tuple<std::string, pugi::xml_document, UniquePtrNotNull<bt::INodeDataSet>>> result;

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
                pugi::xml_parse_result parseResult = doc.load_buffer(data.data(), data.size());
                if (!parseResult)
                {
                    throw std::runtime_error("fail to load xml");
                }

                auto key = path.stem().generic_string();
                auto dataSet = std::make_unique<bt::NodeDataSetXML>(doc.child("root").first_child());

                result.emplace_back(key, std::move(doc), std::move(dataSet));
            }
            catch (const std::exception& e)
            {
                ZEROSUGAR_LOG_ERROR(serviceLocator,
                    std::format("[{}] fail to add behavior tree data. exception: {}, path: {}",
                        GetName(), e.what(), path.generic_string()));
            }
        }

        return result;
    }
}
