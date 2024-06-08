#include "code_generator_factory.h"

#include <cassert>

#include "zerosugar/tool/proto_code_generator/generator/service_code_generator.h"
#include "zerosugar/tool/proto_code_generator/generator/sl_packet_code_generator.h"
#include "zerosugar/tool/proto_code_generator/generator/xr_packet_code_generator.h"

namespace zerosugar
{
    CodeGeneratorFactory::CodeGeneratorFactory()
    {
        Register<ServiceCodeGenerator>();
        Register<SLPacketCodeGenerator>();
        Register<XRPacketCodeGenerator>();
    }

    auto CodeGeneratorFactory::Create(const std::string& writerName) -> std::unique_ptr<ICodeGenerator>
    {
        auto iter = _functions.find(writerName);
        return iter != _functions.end() ? iter->second() : nullptr;
    }

    template <typename T>
    void CodeGeneratorFactory::Register()
    {
        static_assert(std::is_base_of_v<ICodeGenerator, T>);

        [[maybe_unused]] const bool result = Register(
            T{}.GetName(),
            []() -> std::unique_ptr<ICodeGenerator>
            {
                return std::make_unique<T>();
            }
        );
        assert(result);
    }

    bool CodeGeneratorFactory::Register(const std::string& str, const std::function<std::unique_ptr<ICodeGenerator>()>& fn)
    {
        return _functions.try_emplace(str, fn).second;
    }
}
