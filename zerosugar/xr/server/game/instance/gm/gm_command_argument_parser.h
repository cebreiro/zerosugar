#pragma once

namespace zerosugar::xr
{
    class GMCommandArgumentParser
    {
    public:
        template <typename T>
        bool Parse(T& value, const std::span<const std::string>& args, int64_t index) const;

    private:
        bool Parse(int32_t& result, const std::string& arg) const;
        bool Parse(int64_t& result, const std::string& arg) const;
        bool Parse(float& result, const std::string& arg) const;
        bool Parse(double& result, const std::string& arg) const;
        bool Parse(std::string& str, const std::string& arg) const;
    };

    template <typename T>
    bool GMCommandArgumentParser::Parse(T& value, const std::span<const std::string>& args, int64_t index) const
    {
        if constexpr (is_optional<T>)
        {
            if (index >= std::ssize(args))
            {
                value = std::optional<typename T::value_type>(std::nullopt);

                return true;
            }

            return this->Parse(value.emplace(), args[index]);
        }
        else
        {
            if (index >= std::ssize(args))
            {
                return false;
            }

            return this->Parse(value, args[index]);
        }
    }
}
