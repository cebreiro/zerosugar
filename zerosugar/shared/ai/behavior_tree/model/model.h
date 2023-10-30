#pragma once
#include <cassert>
#include <concepts>
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <functional>
#include <unordered_map>
#include <pugixml.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace zerosugar::bt::model
{
    class Param
    {
    public:
        Param() = default;
        virtual ~Param() = default;

        virtual bool Deserialize(const pugi::xml_attribute& attribute);

        template <typename T> requires std::is_arithmetic_v<T>
        static auto As(const pugi::xml_attribute& attribute) -> std::optional<T>;

        template <typename T> requires std::is_base_of_v<Param, T>
        static auto As(const pugi::xml_attribute& attribute) -> std::optional<T>;

        template <typename T> requires std::is_same_v<T, std::vector<typename T::value_type>>
        static auto As(const pugi::xml_attribute& attribute) -> std::optional<T>;

        auto GetName() const -> const std::string&;
        auto GetRawString() const -> const std::string&;

    private:
        std::string _name;
        std::string _rawString;
    };

    class Model
    {
    public:
        Model(const Model& other) = delete;
        Model(Model&& other) noexcept = delete;
        Model& operator=(const Model& other) = delete;
        Model& operator=(Model&& other) noexcept = delete;

        Model() = default;
        virtual ~Model() = default;

        virtual bool Deserialize(const pugi::xml_node& node);

        auto GetName() const -> const std::string&;

    private:
        std::string _name;
    };

    class ModelFactory
    {
    public:
        template <typename T>
        struct Registry
        {
            Registry();
        };

    private:
        ModelFactory() = default;

    public:
        static auto GetInstance() -> ModelFactory&;

        auto CreateModel(const pugi::xml_node& node) -> std::unique_ptr<Model>;

        template <typename T>
        void Register();

    private:
        std::unordered_map<std::string, std::function<std::unique_ptr<Model>()>> _table;
    };

    template <typename T> requires std::is_arithmetic_v<T>
    auto Param::As(const pugi::xml_attribute& attribute) -> std::optional<T>
    {

        try
        {
            return boost::lexical_cast<T>(attribute.as_string());
        }
        catch (...)
        {
        }

        return std::nullopt;
    }

    template <typename T> requires std::is_base_of_v<Param, T>
    auto Param::As(const pugi::xml_attribute& attribute) -> std::optional<T>
    {
        T param;
        if (!param.Deserialize(attribute))
        {
            return std::nullopt;
        }

        return param;
    }

    template <typename T> requires std::is_same_v<T, std::vector<typename T::value_type>>
    auto Param::As(const pugi::xml_attribute& attribute) -> std::optional<T>
    {
        std::string attr = attribute.as_string();
        std::erase_if(attr, ::isspace);

        std::vector<std::string> splits;
        boost::algorithm::split(splits, attr, boost::is_any_of(","));

        std::vector<typename T::value_type> result;

        for (const std::string& str : splits)
        {
            std::optional<typename T::value_type> temp = As<typename T::value_type>();
            if (!temp.has_value())
            {
                return std::nullopt;
            }

            result.push_back(std::move(*temp));
        }

        return result;
    }

    template <typename T>
    ModelFactory::Registry<T>::Registry()
    {
        ModelFactory::GetInstance().Register<T>();
    }

    template <typename T>
    void ModelFactory::Register()
    {
        [[maybe_unused]] const bool inserted = _table.try_emplace(T::class_name, []()
        {
            return std::make_unique<T>();
        }).second;
        assert(inserted);
    }
}
