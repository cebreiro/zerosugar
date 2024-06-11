#pragma once
#include "zerosugar/shared/network/server/server.h"

namespace zerosugar::xr
{
    class ServiceServer : public Server
    {
    public:
        ServiceServer(std::string name, execution::AsioExecutor& executor);

        template <typename T, typename R>
        bool AddProcedure(std::string& name, const std::function<R(T)>& function);

    protected:
        bool AddProcedure(std::string name, const std::function<Future<std::string>(const std::string&)>& function);

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

    private:
        std::unordered_map<std::string, std::function<Future<std::string>(const std::string&)>> _procedures;

        tbb::concurrent_hash_map<session::id_type, Buffer> _sessionReceiveBuffers;
    };

    template <typename T, typename R>
    bool ServiceServer::AddProcedure(std::string& name, const std::function<R(T)>& function)
    {
        return AddProcedure(std::move(name), [function](const std::string& str) -> Future<std::string>
            {
                const nlohmann::json& input = nlohmann::json::parse(str);

                T param;
                from_json(input, param);

                const auto result = co_await function(std::move(param));

                nlohmann::json output;
                to_json(output, result);

                co_return output.dump();
            });
    }
}
