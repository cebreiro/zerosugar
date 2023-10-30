#pragma once
#include <cassert>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include "tools/code_generator/generator/generator_interface.h"

namespace zerosugar
{
    class GeneratorFactory
    {
    private:
        GeneratorFactory() = default;

    public:
        template <typename T>
        struct Registry
        {
            Registry();
        };

    public:
        static auto GetInstance() -> GeneratorFactory&;

        auto CreateGenerator(const std::string& name) const -> std::unique_ptr<ICodeGenerator>;

        template <typename T>
        void Register(const std::string& name);

    private:
        std::unordered_map<std::string, std::function<std::unique_ptr<ICodeGenerator>()>> _table;
    };

    template <typename T>
    GeneratorFactory::Registry<T>::Registry()
    {
        GetInstance().Register<T>(T::name);
    }

    template <typename T>
    void GeneratorFactory::Register(const std::string& name)
    {
        [[maybe_unused]]
        const bool inserted = _table.try_emplace(name, []() -> std::unique_ptr<ICodeGenerator>
            {
                return std::make_unique<T>();
            }).second;
        assert(inserted);
    }
}
