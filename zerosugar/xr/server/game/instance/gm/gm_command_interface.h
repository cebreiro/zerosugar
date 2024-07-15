#pragma once
#include "zerosugar/xr/server/game/instance/gm/gm_command_argument_parser.h"

namespace zerosugar::xr
{
    class GameExecutionSerial;
    class GamePlayerSnapshot;
}

namespace zerosugar::xr
{
    class IGMCommand
    {
    public:
        virtual ~IGMCommand() = default;

        virtual bool Handle(GameExecutionSerial& serialContext, GamePlayerSnapshot& player, const std::span<const std::string>& args) = 0;
    };

    template <typename... TArgs>
    class IGMCommandT : public IGMCommand
    {
    public:
        bool Handle(GameExecutionSerial& serialContext, GamePlayerSnapshot& player, const std::span<const std::string>& args) final
        {
            if constexpr (sizeof...(TArgs) == 0)
            {
                return this->HandleCommand(serialContext, player);
            }
            else
            {
                const auto parse = [&]<typename T, size_t... I>(const GMCommandArgumentParser& parser, T& tuple,
                    const std::span<const std::string>& args, std::index_sequence<I...>) -> bool
                {
                    constexpr auto parseImpl = []<size_t J>(const GMCommandArgumentParser& parser,auto& item, const std::span<const std::string>& args)
                    {
                        return parser.Parse(item, args, static_cast<int64_t>(J));
                    };

                    const bool result = (... && parseImpl.template operator()<I>(parser, std::get<I>(tuple), args));

                    return result;
                };

                std::tuple<TArgs...> tuple;
                GMCommandArgumentParser parser;

                const bool success = parse(parser, tuple, args, std::make_index_sequence<sizeof...(TArgs)>{});
                if (!success)
                {
                    return false;
                }

                return std::apply(std::bind_front(&IGMCommandT::HandleCommand, this, std::ref(serialContext), std::ref(player)), tuple);
            }
        }

    private:
        virtual bool HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player, const TArgs&... args) = 0;
    };
}
