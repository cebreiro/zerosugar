#include "proto_code_generator_option.h"

namespace zerosugar::sl
{
    void from_json(const nlohmann::json& j, ProtoCodeGeneratorOption& item)
    {
        j.at("include_path").get_to(item.includePath);
        j.at("generator").get_to(item.generator);
    }
}
