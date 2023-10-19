#include "server.h"

#include <cassert>

#include "zerosugar/shared/execution/channel/async_enumerable.h"
#include "zerosugar/shared/execution/future/future.hpp"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/operation/post.h"
#include "zerosugar/shared/network/session/session.h"

namespace zerosugar
{
    Server::Server(execution::AsioExecutor& executor)
        : _executor(executor)
        , _channel(std::make_shared<server::event_channel_type>())
    {
    }

    bool Server::StartUp(uint16_t listenPort)
    {
        assert(!_acceptor.has_value());

        _listenPort = listenPort;

        try
        {
            constexpr bool reuseAddress = false;
            _acceptor.emplace(_executor.GetIoContext(),
                boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), _listenPort),
                reuseAddress);
        }
        catch (...)
        {
        }

        if (!_acceptor.has_value() || !_acceptor->is_open())
        {
            return false;
        }

        const int64_t concurrency = _executor.GetConcurrency();
        assert(concurrency > 0);

        for (int64_t i = 0; i < std::max<int64_t>(1, concurrency); ++i)
        {
            Post(_executor, [this]()
                {
                    Run();
                });
        }

        AcceptAsync();

        return true;
    }

    void Server::Shutdown()
    {
        assert(_acceptor.has_value());

        boost::system::error_code ec;
        _acceptor->cancel(ec);

        _channel->Close();
    }

    auto Server::GetListenPort() const -> uint16_t
    {
        return _listenPort;
    }

    void Server::AcceptAsync()
    {
        assert(_acceptor.has_value());
        assert(_acceptor->is_open());

        _acceptor->async_accept(
            [self = shared_from_this()](const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket)
            {
                self->OnAccept(ec, std::move(socket));
            });
    }

    void Server::OnAccept(const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket)
    {
        using namespace zerosugar::server;

        if (ec)
        {
            if (ec == boost::asio::error::operation_aborted)
            {
                return;
            }

            _channel->Send(AcceptError{ .errorCode = ec }, channel::ChannelSignal::NotifyOne);
        }
        else
        {
            _channel->Send(AcceptEvent{ .socket = std::move(socket) }, channel::ChannelSignal::NotifyOne);
        }

        AcceptAsync();
    }

    auto Server::Run() -> Future<void>
    {
        const auto holder = shared_from_this();
        (void)holder;

        AsyncEnumerable<server::event_type> enumerable(_channel);

        while (enumerable.HasNext())
        {
            server::event_type event = co_await enumerable;

            std::visit([this]<typename T>(T && arg)
            {
                if constexpr (std::is_same_v<T, std::monostate>)
                {
                    assert(false);
                }
                else if constexpr (std::is_same_v<T, server::AcceptEvent>)
                {
                    session::id_type id = PublishSessionId();
                    auto session = std::make_shared<Session>(id, _channel, std::move(arg.socket),
                        make_strand(_executor.GetIoContext()));

                    decltype(_sessions)::accessor accessor;
                    if (_sessions.insert(accessor, id))
                    {
                        accessor->second = session;
                    }
                    else
                    {
                        session->Close();
                        assert(false);

                        return;
                    }

                    session->StartReceive();
                }
                else if constexpr (std::is_same_v<T, server::AcceptError>)
                {

                }
                else if constexpr (std::is_same_v<T, server::SessionReceiveEvent>)
                {
                }
                else if constexpr (std::is_same_v<T, server::SessionIoErrorEvent>)
                {
                }
                else if constexpr (std::is_same_v<T, server::SessionDestructEvent>)
                {
                    _sessionIdRecycleQueue.push(arg.id.Unwrap());
                }
                else
                {
                    static_assert(!sizeof(T), "not implemented");
                }
            }, std::move(event));
        }
    }

    auto Server::PublishSessionId() -> session::id_type
    {
        session::id_type::value_type result = session::id_type::Default().Unwrap();
        if (_sessionIdRecycleQueue.try_pop(result))
        {
            return session::id_type(result);
        }

        return session::id_type(++_nextSessionId);
    }
}
