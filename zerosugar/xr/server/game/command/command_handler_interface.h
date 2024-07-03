#pragma once

namespace zerosugar::xr
{
    class GameServer;

    class ICommandHandler
    {
    public:
        virtual ~ICommandHandler() = default;

        virtual auto Handle(GameServer& server, const std::string& str, std::optional<int64_t> responseId) const -> Future<void> = 0;
    };

    template <typename T>
    class CommandHandlerT : public ICommandHandler
    {
    public:
        static constexpr auto opcode = T::opcode;

    public:
        auto Handle(GameServer& server, const std::string& str, std::optional<int64_t> responseId) const -> Future<void> final
        {
            return this->HandleCommand(server, nlohmann::json::parse(str).get<T>(), responseId);
        }

    private:
        virtual auto HandleCommand(GameServer& server, T command, std::optional<int64_t> responseId) const -> Future<void> = 0;
    };
}
