#pragma once

namespace zerosugar::xr
{
    class CoordinationService;
}

namespace zerosugar::xr::coordination
{
    class GameServer;

    class ICommandResponseHandler
    {
    public:
        virtual ~ICommandResponseHandler() = default;

        virtual auto Handle(CoordinationService& service, GameServer& server, const std::string& str) const -> Future<void> = 0;
    };

    template <typename T>
    class CommandResponseHandlerT : public ICommandResponseHandler
    {
    public:
        static constexpr auto opcode = T::opcode;

    public:
        auto Handle(CoordinationService& service, GameServer& server, const std::string& str) const -> Future<void> final
        {
            const nlohmann::json& json = nlohmann::json::parse(str);
            const T& response = json.get<T>();

            return this->HandleResponse(service, server, response);
        }

    private:
        virtual auto HandleResponse(CoordinationService& service, GameServer& server, const T& response) const -> Future<void> = 0;
    };

}
