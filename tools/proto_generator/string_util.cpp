#include "string_util.h"

#include <boost/algorithm/string.hpp>

namespace zerosugar::string_util
{
    auto ToSnakeCase(const std::string& str) -> std::string
    {
        std::string temp(str.begin(), str.end());

        for (int64_t i = 1; i < std::ssize(temp); ++i)
        {
            if (::isupper(temp[i]))
            {
                temp.insert(i, "_");
                ++i;
            }
        }

        return boost::algorithm::to_lower_copy(temp);
    }
}
