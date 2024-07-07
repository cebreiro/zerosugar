#include "gm_command_argument_parser.h"

#include <boost/lexical_cast.hpp>

namespace zerosugar::xr
{
    namespace detail::game
    {
        template <typename T>
        bool Parse(T& value, const std::string& arg)
        {
            try
            {
                value = boost::lexical_cast<T>(arg);

                return true;
            }
            catch (...)
            {
            }

            return false;
        }
    }

    bool GMCommandArgumentParser::Parse(int32_t& result, const std::string& arg) const
    {
        return detail::game::Parse(result, arg);
    }

    bool GMCommandArgumentParser::Parse(int64_t& result, const std::string& arg) const
    {
        return detail::game::Parse(result, arg);
    }

    bool GMCommandArgumentParser::Parse(float& result, const std::string& arg) const
    {
        return detail::game::Parse(result, arg);
    }

    bool GMCommandArgumentParser::Parse(double& result, const std::string& arg) const
    {
        return detail::game::Parse(result, arg);
    }

    bool GMCommandArgumentParser::Parse(std::string& str, const std::string& arg) const
    {
        str = arg;

        return true;
    }
}
