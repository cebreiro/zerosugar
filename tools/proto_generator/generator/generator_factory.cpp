#include "generator_factory.h"

namespace zerosugar
{
    auto GeneratorFactory::GetInstance() -> GeneratorFactory&
    {
        static GeneratorFactory instance;
        return instance;
    }

    auto GeneratorFactory::CreateGenerator(const std::string& name) const -> std::unique_ptr<ICodeGenerator>
    {
        auto iter = _table.find(name);
        return iter != _table.end() ? iter->second() : nullptr;
    }
}
