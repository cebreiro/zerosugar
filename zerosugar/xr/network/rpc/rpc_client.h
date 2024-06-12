#pragma once
#include <boost/callable_traits.hpp>
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/xr/network/model/generated/rpc_generated.h"

namespace zerosugar
{
    class Socket;
}

namespace zerosugar::execution
{
    class AsioExecutor;
    class AsioStrand;
}

namespace zerosugar::xr::service
{
}

namespace zerosugar::xr
{
    class RPCClient final
        : public IService
        , public std::enable_shared_from_this<RPCClient>
    {
        using send_callback_type = std::function<void(network::RemoteProcedureCallErrorCode, const std::string&)>;

    public:
        explicit RPCClient(SharedPtrNotNull<execution::AsioExecutor> executor);

        auto ConnectAsync(std::string address, uint16_t port) -> Future<void>;

        auto RegisterToServer(std::string serviceName, std::string ip, uint16_t port) -> Future<void>;

        template <typename T, typename Func>
        bool RegisterProcedure(const std::string& rpcName, const Func& function);

        template <typename T, typename P, typename R>
        auto CallRemoteProcedure(std::string rpcName, const P& param) -> Future<R>;

    private:
        auto Run() -> Future<void>;
        void Send(int32_t rpcId, std::string serviceName, std::string rpcName, std::string param, const send_callback_type& callback);

        auto HandleRequestRemoteProcedureCall(network::RequestRemoteProcedureCall request) -> Future<void>;
        void HandleResultRemoteProcedureCall(const network::ResultRemoteProcedureCall& result);

    private:
        static auto MakeProcedureKey(const std::string& service, const std::string& rpc) -> std::string;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<execution::AsioStrand> _strand;

        SharedPtrNotNull<Socket> _socket;

        std::unordered_map<std::string, Promise<void>> _registers;
        std::unordered_map<std::string, std::function<Future<std::string>(const std::string&)>> _procedures;

        std::unordered_map<std::string, int32_t> _nextRemoteProcedureIds;
        std::unordered_map<std::string, std::unordered_map<int32_t, send_callback_type>> _remoteProcedures;
    };

    template <typename T, typename Func>
    bool RPCClient::RegisterProcedure(const std::string& rpcName, const Func& function)
    {
        using TParam = std::tuple_element_t<0, boost::callable_traits::args_t<Func>>;
        using TResult = boost::callable_traits::return_type_t<Func>;

        return _procedures.try_emplace(MakeProcedureKey(T::name, rpcName), [function](const std::string& str) -> Future<std::string>
            {
                const nlohmann::json& input = nlohmann::json::parse(str);
                TParam param = input.get<TParam>();

                const typename TResult::value_type result = co_await function(std::move(param));
                nlohmann::json output = result;

                co_return output.dump();
            }).second;
    }

    template <typename T, typename P, typename R>
    auto RPCClient::CallRemoteProcedure(std::string rpcName, const P& param) -> Future<R>
    {
        const nlohmann::json input = param;
        std::string str = input.dump();

        co_await *_strand;

        Promise<R> promise;
        Future<R> future = promise.GetFuture();

        const int32_t rpcId = ++_nextRemoteProcedureIds[T::name];

        this->Send(rpcId, T::name, rpcName, std::move(str),
            [p = std::move(promise), rpcId, rpcName](network::RemoteProcedureCallErrorCode ec, const std::string& param) mutable
            {
                if (ec != network::RemoteProcedureCallErrorCode::RpcErrorNone)
                {
                    try
                    {
                        throw std::runtime_error(std::format("rpc error. rpc_id: {}, rpc_name: {}::{}, error_code: {}",
                            rpcId, T::name, rpcName, GetEnumName(ec)));
                    }
                    catch (...)
                    {
                        p.SetException(std::current_exception());
                    }
                }
                else
                {
                    nlohmann::json j = nlohmann::json::parse(param);

                    R result = j.get<R>();

                    p.Set(std::move(result));
                }
            });

        co_return co_await future;
    }
}
