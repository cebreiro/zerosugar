#pragma once
#include <boost/callable_traits.hpp>
#include <boost/scope_exit.hpp>
#include <boost/scope/scope_exit.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/snowflake/snowflake.h"
#include "zerosugar/xr/network/model/generated/rpc_message.h"

namespace zerosugar
{
    class Socket;
}

namespace zerosugar::execution
{
    class AsioExecutor;
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
        using server_streaming_callback = std::function<void(const std::string&)>;
        using completion_callback_type = std::function<void(network::RemoteProcedureCallErrorCode, const std::string&)>;

    public:
        explicit RPCClient(SharedPtrNotNull<execution::AsioExecutor> executor);

        void Initialize(ServiceLocator& serviceLocator) override;
        auto ScheduleCompletionLog(std::chrono::milliseconds duration) -> Future<void>;

        auto ConnectAsync(std::string address, uint16_t port) -> Future<void>;

        auto RegisterToServer(std::string serviceName) -> Future<void>;

        template <bool ParamStreaming, bool ReturnStreaming, typename Func>
        bool RegisterProcedure(const std::string& serviceName, const std::string& rpcName, const Func& function);

        template <typename P, typename R>
        auto CallRemoteProcedure(std::string serviceName, std::string rpcName, P param) -> Future<R>;

        template <typename P, typename R>
        auto CallRemoteProcedureClientStreaming(std::string serviceName, std::string rpcName, AsyncEnumerable<P> enumerable) -> Future<R>;

        template <typename P, typename R>
        auto CallRemoteProcedureServerStreaming(std::string serviceName, std::string rpcName, const P& param) -> AsyncEnumerable<R>;

        template <typename P, typename R>
        auto CallRemoteProcedureClientServerStreaming(std::string serviceName, std::string rpcName, AsyncEnumerable<P> enumerable) -> AsyncEnumerable<R>;

        auto GetName() const -> std::string_view override;

    private:
        auto Run() -> Future<void>;

        bool HasCompletionCallback(const std::string& serviceName, int64_t rpcId) const;
        void SetCompletionCallback(const std::string& serviceName, int64_t rpcId, const completion_callback_type& callback);

        void SetServerStreamingCallback(const std::string& serviceName, int64_t rpcId, const server_streaming_callback& callback);

        void Send(int64_t rpcId, std::string serviceName, std::string rpcName, std::string param);
        void SendClientStreaming(int64_t rpcId, std::string serviceName, std::string param);
        void SendAbortClientStreaming(int64_t rpcId, const std::string& serviceName);

        auto HandleRequestRemoteProcedureCall(network::RequestRemoteProcedureCall request) -> Future<void>;
        void HandleResultRemoteProcedureCall(const network::ResultRemoteProcedureCall& result);
        void HandleServerStreaming(const network::SendServerStreaming& serverStreaming);
        void HandleClientStreaming(const network::SendClientSteaming& clientStreaming);
        void HandleAbortClientStreaming(const network::AbortClientStreamingRPC& abort);

    private:
        auto GenerateRPCId() -> int64_t;

        static auto MakeProcedureKey(const std::string& service, const std::string& rpc) -> std::string;
        static auto GetRPCFullName(const std::string& service, const std::string& rpc) -> std::string;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<execution::AsioStrand> _strand;
        ServiceLocatorT<ILogService> _serviceLocator;

        SharedPtrNotNull<Socket> _socket;
        bool _logLogScheduled = false;

        Promise<void> _snowflakePromise;
        std::optional<UniqueSnowflake<>> _snowflake = std::nullopt;;

        // provider-side
        std::unordered_map<std::string, Promise<void>> _registers;
        std::unordered_map<std::string,
            std::variant<
                std::function<Future<std::string>(const std::string&)>,
                std::function<AsyncEnumerable<std::string>(const std::string&)>,
                std::function<Future<std::string>(SharedPtrNotNull<Channel<std::string>>)>,
                std::function<AsyncEnumerable<std::string>(SharedPtrNotNull<Channel<std::string>>)>
            >
        > _procedures;
        std::unordered_map<int64_t, SharedPtrNotNull<Channel<std::string>>> _runningClientStreamingProcedures;
        boost::unordered::unordered_flat_map<std::string, int32_t> _completeRPCCounts;

        // client-side
        std::unordered_map<std::string,
            std::unordered_map<int64_t, std::pair<server_streaming_callback, completion_callback_type>>> _remoteProcedures;
        boost::unordered::unordered_flat_map<std::string, int32_t> _receiveCompleteRPCCounts;
    };

    template <bool ParamStreaming, bool ReturnStreaming, typename Func>
    bool RPCClient::RegisterProcedure(const std::string& serviceName, const std::string& rpcName, const Func& function)
    {
        using TParam = std::tuple_element_t<0, boost::callable_traits::args_t<Func>>;
        using TResult = boost::callable_traits::return_type_t<Func>;

        if constexpr (ReturnStreaming)
        {
            if constexpr (ParamStreaming)
            {
                return _procedures.try_emplace(MakeProcedureKey(serviceName, rpcName),
                    [function](SharedPtrNotNull<Channel<std::string>> externChannel) -> AsyncEnumerable<std::string>
                    {
                        execution::IExecutor* executor = ExecutionContext::GetExecutor();
                        assert(executor);

                        auto externContext = std::make_shared<future::SharedContext<void>>();
                        auto outputContext = std::make_shared<future::SharedContext<void>>();

                        auto inputChannel = std::make_shared<Channel<typename TParam::value_type>>();
                        auto outputChannel = function(AsyncEnumerable<typename TParam::value_type>(inputChannel)).GetChannel();

                        externChannel->AddOrExecuteSignalHandler([externContext]()
                            {
                                externContext->OnSuccess();
                            });

                        outputChannel->AddOrExecuteSignalHandler([outputContext]()
                            {
                                outputContext->OnSuccess();
                            });

                        while (true)
                        {
                            co_await WaitAny(*executor, Future(externContext), Future(outputContext));
                            ExecutionContext::IsEqualTo(*executor);

                            const bool completeExternContext = externContext->IsComplete();
                            const bool completeOutputContext = outputContext->IsComplete();

                            externContext->Reset();
                            outputContext->Reset();

                            if (completeExternContext)
                            {
                                std::variant<std::string, std::exception_ptr> item;

                                while (externChannel->TryPop(item))
                                {
                                    const std::string str = std::visit([]<typename T>(T&& item) -> std::string
                                    {
                                        if constexpr (std::is_same_v<T, std::string>)
                                        {
                                            return item;
                                        }
                                        else
                                        {
                                            std::rethrow_exception(item);
                                        }

                                    }, std::move(item));

                                    const nlohmann::json& input = nlohmann::json::parse(str);
                                    typename TParam::value_type param = input.get<typename TParam::value_type>();

                                    inputChannel->Send(std::move(param), channel::ChannelSignal::NotifyOne);
                                }

                                if (externChannel->IsOpen())
                                {
                                    externChannel->AddOrExecuteSignalHandler([externContext]()
                                        {
                                            externContext->OnSuccess();
                                        });
                                }
                                else
                                {
                                    inputChannel->Close();
                                }
                            }

                            if (completeOutputContext)
                            {
                                std::variant<typename TResult::value_type, std::exception_ptr> item;

                                while (outputChannel->TryPop(item))
                                {
                                    const std::string str = std::visit([]<typename T>(T&& item) -> std::string
                                    {
                                        if constexpr (std::is_same_v<T, typename TResult::value_type>)
                                        {
                                            nlohmann::json output = item;
                                            return output.dump();
                                        }
                                        else
                                        {
                                            std::rethrow_exception(item);
                                        }

                                    }, std::move(item));

                                    co_yield str;
                                }

                                if (outputChannel->IsOpen())
                                {
                                    outputChannel->AddOrExecuteSignalHandler([outputContext]()
                                        {
                                            outputContext->OnSuccess();
                                        });
                                }
                                else
                                {
                                    co_return;
                                }
                            }
                        }

                    }).second;
            }
            else
            {
                return _procedures.try_emplace(MakeProcedureKey(serviceName, rpcName),
                    [function](const std::string& str) -> AsyncEnumerable<std::string>
                    {
                        const nlohmann::json& input = nlohmann::json::parse(str);
                        TParam param = input.get<TParam>();

                        AsyncEnumerable<typename TResult::value_type> enumerable = function(std::move(param));

                        while (enumerable.HasNext())
                        {
                            const typename TResult::value_type result = co_await enumerable;

                            nlohmann::json output = result;

                            co_yield output.dump();
                        }

                    }).second;
            }
        }
        else
        {
            if constexpr (ParamStreaming)
            {
                return _procedures.try_emplace(MakeProcedureKey(serviceName, rpcName),
                    [function](SharedPtrNotNull<Channel<std::string>> externChannel) -> Future<std::string>
                    {
                        execution::IExecutor* executor = ExecutionContext::GetExecutor();
                        assert(executor);

                        auto inputChannel = std::make_shared<Channel<typename TParam::value_type>>();
                        Future<typename TResult::value_type> future = function(AsyncEnumerable<typename TParam::value_type>(inputChannel));

                        auto externContext = std::make_shared<future::SharedContext<void>>();
                        externChannel->AddOrExecuteSignalHandler([externContext]()
                            {
                                externContext->OnSuccess();
                            });

                        auto outputContext = std::make_shared<future::SharedContext<void>>();
                        future.ContinuationWith(*executor, [outputContext](Future<typename TResult::value_type>& self) -> Future<void>
                            {
                                (void)self;

                                outputContext->OnSuccess();

                                co_return;
                            });

                        while (true)
                        {
                            co_await WaitAny(*executor, Future(externContext), Future(outputContext));

                            const bool completeExternContext = externContext->IsComplete();
                            const bool completeOutputContext = outputContext->IsComplete();

                            if (completeOutputContext)
                            {
                                const typename TResult::value_type result = future.Get();
                                nlohmann::json output = result;

                                co_return output.dump();
                            }

                            if (completeExternContext)
                            {
                                std::variant<std::string, std::exception_ptr> item;

                                while (externChannel->TryPop(item))
                                {
                                    const std::string str = std::visit([]<typename T>(T&& item) -> std::string
                                    {
                                        if constexpr (std::is_same_v<T, std::string>)
                                        {
                                            return item;
                                        }
                                        else
                                        {
                                            std::rethrow_exception(item);
                                        }

                                    }, std::move(item));

                                    const nlohmann::json& input = nlohmann::json::parse(str);
                                    typename TParam::value_type param = input.get<typename TParam::value_type>();

                                    if (inputChannel->IsOpen())
                                    {
                                        inputChannel->Send(std::move(param), channel::ChannelSignal::NotifyOne);
                                    }
                                }

                                externContext->Reset();
                                outputContext->Reset();

                                if (externChannel->IsOpen())
                                {
                                    externChannel->AddOrExecuteSignalHandler([externContext]()
                                        {
                                            externContext->OnSuccess();
                                        });
                                }
                                else
                                {
                                    inputChannel->Close();
                                }
                            }
                        }

                    }).second;
            }
            else
            {
                return _procedures.try_emplace(MakeProcedureKey(serviceName, rpcName),
                    [function](const std::string& str) -> Future<std::string>
                    {
                        const nlohmann::json& input = nlohmann::json::parse(str);
                        TParam param = input.get<TParam>();

                        const typename TResult::value_type result = co_await function(std::move(param));
                        nlohmann::json output = result;

                        co_return output.dump();

                    }).second;
            }
        }
    }

    template <typename P, typename R>
    auto RPCClient::CallRemoteProcedure(std::string serviceName, std::string rpcName, P param) -> Future<R>
    {
        assert(!serviceName.empty() && !rpcName.empty());

        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;

        const int64_t rpcId = GenerateRPCId();

        Promise<R> promise;
        Future<R> future = promise.GetFuture();

        this->SetCompletionCallback(serviceName, rpcId,
            [p = std::move(promise), rpcId, serviceName, rpcName](network::RemoteProcedureCallErrorCode ec, const std::string& param) mutable
            {
                if (ec != network::RemoteProcedureCallErrorCode::RpcErrorNone)
                {
                    try
                    {
                        throw std::runtime_error(fmt::format("rpc error. rpc_id: {}, rpc_name: {}::{}, error_code: {}",
                            rpcId, serviceName, rpcName, GetEnumName(ec)));
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

        const nlohmann::json input = param;
        std::string str = input.dump();

        this->Send(rpcId, serviceName, rpcName, std::move(str));

        co_return co_await future;
    }

    template <typename P, typename R>
    auto RPCClient::CallRemoteProcedureClientStreaming(std::string serviceName, std::string rpcName, AsyncEnumerable<P> enumerable) -> Future<R>
    {
        assert(!serviceName.empty() && !rpcName.empty());

        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;

        const int64_t rpcId = GenerateRPCId();

        Promise<R> promise;
        Future<R> future = promise.GetFuture();

        this->SetCompletionCallback(serviceName, rpcId,
            [p = std::move(promise), rpcId, serviceName, rpcName, channel = enumerable.GetChannel()]
            (network::RemoteProcedureCallErrorCode ec, const std::string& param) mutable
            {
                if (ec != network::RemoteProcedureCallErrorCode::RpcErrorNone)
                {
                    assert(ec != network::RemoteProcedureCallErrorCode::RpcErrorStreamingClosedGracefully);

                    try
                    {
                        throw std::runtime_error(fmt::format("rpc error. rpc_id: {}, rpc_name: {}::{}, error_code: {}",
                            rpcId, serviceName, rpcName, GetEnumName(ec)));
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

                if (channel->IsOpen())
                {
                    channel->Close();
                }
            });

        try
        {
            bool first = true;

            while (enumerable.HasNext())
            {
                const P& item = co_await enumerable;
                const nlohmann::json json = item;
                std::string str = json.dump();

                if (first)
                {
                    this->Send(rpcId, serviceName, rpcName, std::move(str));

                    first = false;
                }
                else
                {
                    this->SendClientStreaming(rpcId, serviceName, std::move(str));
                }
            }
        }
        catch (const AsyncEnumerableClosedException& e)
        {
            (void)e;

            this->SendAbortClientStreaming(rpcId, serviceName);
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_ERROR(_serviceLocator,
                fmt::format("[rpc_client] call client streaming rpc channel throws. service: {}, rpc: {}, exception: {}",
                    serviceName, rpcName, e.what()));

            this->SendAbortClientStreaming(rpcId, serviceName);
        }

        co_return co_await future;
    }

    template <typename P, typename R>
    auto RPCClient::CallRemoteProcedureServerStreaming(std::string serviceName, std::string rpcName, const P& param) -> AsyncEnumerable<R>
    {
        assert(!serviceName.empty() && !rpcName.empty());

        const nlohmann::json input = param;
        std::string str = input.dump();

        auto channel = std::make_shared<Channel<R>>();
        AsyncEnumerable<R> result(channel);

        Post(*_strand, [](SharedPtrNotNull<RPCClient> self, std::string param, SharedPtrNotNull<Channel<R>> channel,
            std::string serviceName, std::string rpcName)
            {
                const int64_t rpcId = self->GenerateRPCId();

                self->SetServerStreamingCallback(serviceName, rpcId, [channel = channel](const std::string& str)
                    {
                        if (channel->IsOpen())
                        {
                            nlohmann::json j = nlohmann::json::parse(str);
                            R result = j.get<R>();

                            channel->Send(std::move(result), channel::ChannelSignal::NotifyOne);
                        }
                    });

                self->SetCompletionCallback(serviceName, rpcId,
                    [channel = std::move(channel), rpcId, serviceName, rpcName](network::RemoteProcedureCallErrorCode ec, const std::string& param) mutable
                    {
                        (void)param;

                        if (ec == network::RemoteProcedureCallErrorCode::RpcErrorStreamingClosedGracefully ||
                            ec == network::RemoteProcedureCallErrorCode::RpcErrorNone)
                        {
                            channel->Close();
                        }
                        else
                        {
                            try
                            {
                                throw std::runtime_error(fmt::format("rpc error. rpc_id: {}, rpc_name: {}::{}, error_code: {}",
                                    rpcId, serviceName, rpcName, GetEnumName(ec)));
                            }
                            catch (...)
                            {
                                if (channel->IsOpen())
                                {
                                    channel->Send(std::current_exception(), channel::ChannelSignal::NotifyOne);
                                }
                            }
                        }
                    });

                self->Send(rpcId, serviceName, rpcName, param);

            }, shared_from_this(), std::move(str), std::move(channel), std::move(serviceName), std::move(rpcName));

        return result;
    }

    template <typename P, typename R>
    auto RPCClient::CallRemoteProcedureClientServerStreaming(std::string serviceName, std::string rpcName, AsyncEnumerable<P> enumerable) -> AsyncEnumerable<R>
    {
        assert(!serviceName.empty() && !rpcName.empty());

        auto channel = std::make_shared<Channel<R>>();
        AsyncEnumerable<R> result(channel);

        Post(*_strand, [](SharedPtrNotNull<RPCClient> self, AsyncEnumerable<P> enumerable, SharedPtrNotNull<Channel<R>> channel,
            std::string serviceName, std::string rpcName) -> Future<void>
            {
                const int64_t rpcId = self->GenerateRPCId();

                self->SetServerStreamingCallback(serviceName, rpcId, [channel = channel](const std::string& str)
                    {
                        if (channel->IsOpen())
                        {
                            nlohmann::json j = nlohmann::json::parse(str);
                            R result = j.get<R>();

                            channel->Send(std::move(result), channel::ChannelSignal::NotifyOne);
                        }
                    });

                self->SetCompletionCallback(serviceName, rpcId,
                    [channel = std::move(channel), rpcId, serviceName, rpcName](network::RemoteProcedureCallErrorCode ec, const std::string& param) mutable
                    {
                        (void)param;

                        if (ec == network::RemoteProcedureCallErrorCode::RpcErrorStreamingClosedGracefully ||
                            ec == network::RemoteProcedureCallErrorCode::RpcErrorNone)
                        {
                            channel->Close();
                        }
                        else
                        {
                            try
                            {
                                throw std::runtime_error(fmt::format("rpc error. rpc_id: {}, rpc_name: {}::{}, error_code: {}",
                                    rpcId, serviceName, rpcName, GetEnumName(ec)));
                            }
                            catch (...)
                            {
                                if (channel->IsOpen())
                                {
                                    channel->Send(std::current_exception(), channel::ChannelSignal::NotifyOne);
                                }
                            }
                        }
                    });

                try
                {
                    bool first = true;

                    while (enumerable.HasNext())
                    {
                        const P& item = co_await enumerable;
                        const nlohmann::json json = item;
                        std::string str = json.dump();

                        if (first)
                        {
                            self->Send(rpcId, serviceName, rpcName, std::move(str));

                            first = false;
                        }
                        else
                        {
                            self->SendClientStreaming(rpcId, serviceName, std::move(str));
                        }
                    }
                }
                catch (const AsyncEnumerableClosedException& e)
                {
                    (void)e;

                    self->SendAbortClientStreaming(rpcId, serviceName);
                }
                catch (const std::exception& e)
                {
                    ZEROSUGAR_LOG_ERROR(self->_serviceLocator,
                        fmt::format("[rpc_client] call client streaming rpc channel throws. service: {}, rpc: {}, exception: {}",
                            serviceName, rpcName, e.what()));

                    self->SendAbortClientStreaming(rpcId, serviceName);
                }

            }, shared_from_this(), std::move(enumerable), std::move(channel), std::move(serviceName), std::move(rpcName));

        return result;
    }
}
