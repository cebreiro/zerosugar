#pragma once

namespace zerosugar::xr
{
    class GameServer;

    class ICommandHandler
    {
    public:
        virtual ~ICommandHandler() = default;

        virtual auto Handle(GameServer& server, const std::string& str) const -> Future<void> = 0;
    };

    template <typename T>
    class CommandHandlerT : public ICommandHandler
    {
    public:
        static constexpr auto opcode = T::opcode;

    public:
        auto Handle(GameServer& server, const std::string& str) const -> Future<void> final
        {
            const T& command = nlohmann::json(str).get<T>();

            return this->HandleCommand(server, command);
        }

    private:
        virtual auto HandleCommand(GameServer& server, const T& command) const -> Future<void> = 0;
    };
}
