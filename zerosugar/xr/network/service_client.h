#pragma once
#include "zerosugar/xr/network/model/generated/service_to_service_generated.h"

namespace zerosugar
{
    class Socket;
}

namespace zerosugar::execution
{
    class AsioStrand;
}

namespace zerosugar::xr::network::service
{
    struct ResultRemoteProcedureCall;
}

namespace zerosugar::xr
{
    class ServiceClient
    {
        using send_callback_type = std::function<void(network::service::RemoteProcedureCallErrorCode, const std::string&)>;

    public:
        explicit ServiceClient(SharedPtrNotNull<execution::AsioStrand> strand);

        auto ConnectAsync(std::string address, uint16_t port) -> Future<void>;

        template <typename T, typename R>
        auto CallRemoteProcedure(std::string rpcName, const T& param) -> Future<std::expected<R, network::service::RemoteProcedureCallErrorCode>>;

    private:
        auto Run() -> Future<void>;
        void Send(int32_t rpcId, std::string rpcName, std::string param, const send_callback_type& callback);

        void HandleRPCResult(const network::service::ResultRemoteProcedureCall& result);

    private:
        void ExpandReceiveBuffer(int64_t minSize);
        void ExpandSendBuffer();

    private:
        SharedPtrNotNull<execution::AsioStrand> _strand;

        SharedPtrNotNull<Socket> _socket;
        Buffer _sendBuffer;
        Buffer _receiveBuffer;

        int32_t _nextRPCIds = 0;
        std::unordered_map<int64_t, send_callback_type> _rpcs;
    };

    template <typename T, typename R>
    auto ServiceClient::CallRemoteProcedure(std::string rpcName, const T& param) -> Future<std::expected<R, network::service::RemoteProcedureCallErrorCode>>
    {
        std::string str = to_json(param).dump();

        co_await _strand;

        Promise<R> promise;
        Future<R> future = promise.GetFuture();

        int64_t rpcId = ++_nextRPCIds;

        this->Send(rpcId, std::move(rpcName), std::move(str),
            [p = std::move(promise)](network::service::RemoteProcedureCallErrorCode ec, const std::string& param) mutable
            {
                if (ec != network::service::RemoteProcedureCallErrorCode::RpcErrorNone)
                {
                    p.Set(std::unexpected(ec));
                }
                else
                {
                    nlohmann::json j = nlohmann::json::parse(param);

                    R result = {};
                    from_json(j, result);

                    p.Set(result);
                }
            });

        co_return co_await future;
    }
}
