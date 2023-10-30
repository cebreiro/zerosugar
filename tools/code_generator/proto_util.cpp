#include "proto_util.h"

#include <boost/algorithm/string.hpp>

namespace zerosugar::proto_util
{
    auto ConvertPackageToNamespace(const std::string& package) -> std::string
    {
        std::string result(package);

        boost::algorithm::replace_all(result, ".", "::");
        boost::algorithm::to_lower(result);

        return result;
    }
}
