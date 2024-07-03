#pragma once
#include <any>
#include <string>
#include <unordered_map>

namespace zerosugar::bt
{
    class BlackBoard
    {
    public:
        void Clear();

        bool Contains(const std::string& key) const;

        template <typename T>
        bool ContainsAs(const std::string& key) const;

        template <typename T>
        bool Insert(const std::string& key, const T& value);

        template <typename T>
        void InsertOrUpdate(const std::string& key, const T& value);

        template <typename T>
        auto Get(const std::string& key) -> T&;

        template <typename T>
        auto GetIf(const std::string& key) -> T*;

    private:
        std::unordered_map<std::string, std::any> _container;
    };

    template <typename T>
    bool BlackBoard::ContainsAs(const std::string& key) const
    {
        const auto iter = _container.find(key);
        if (iter == _container.end())
        {
            return false;
        }

        return typeid(T) == iter->second.type();
    }

    template <typename T>
    bool BlackBoard::Insert(const std::string& key, const T& value)
    {
        return _container.try_emplace(key, value).second;
    }

    template <typename T>
    void BlackBoard::InsertOrUpdate(const std::string& key, const T& value)
    {
        _container[key] = value;
    }

    template <typename T>
    auto BlackBoard::Get(const std::string& key) -> T&
    {
        T* result = GetIf<T>(key);
        assert(result);

        return *result;
    }

    template <typename T>
    auto BlackBoard::GetIf(const std::string& key) -> T*
    {
        const auto iter = _container.find(key);
        if (iter == _container.end())
        {
            return nullptr;
        }

        return std::any_cast<T>(&iter->second);
    }
}
