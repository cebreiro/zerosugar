#pragma once
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

namespace zerosugar::sl
{
    class ICodeGenerator;

    class CodeGeneratorFactory
    {
    public:
        CodeGeneratorFactory();

        auto Create(const std::string& writerName) -> std::unique_ptr<ICodeGenerator>;

    private:
        template <typename T>
        void Register();
        bool Register(const std::string& str, const std::function<std::unique_ptr<ICodeGenerator>()>& fn);

    private:
        std::unordered_map<std::string, std::function<std::unique_ptr<ICodeGenerator>()>> _functions;
    };
}
