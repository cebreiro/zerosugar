#include "server.h"

#include <cassert>

#include "zerosugar/shared/execution/channel/async_enumerable.h"
#include "zerosugar/shared/execution/future/future.hpp"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/execution/executor/operation/post.h"
#include "zerosugar/shared/network/session/session.h"

namespace zerosugar
{
    Server::Server(execution::AsioExecutor& executor)
        : _executor(executor)
        , _externalChannel(std::make_shared<server::event_channel_type>())
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

        AcceptAsync();

        return true;
    }

    void Server::Shutdown()
    {
        assert(_acceptor.has_value());

        boost::system::error_code ec;
        _acceptor->cancel(ec);

        {
            std::lock_guard lock(_sessionMutex);
            for (const auto& session : _sessions | std::views::values)
            {
                session->Close();
            }
        }

        _externalChannel->Close();
    }

    bool Server::IsOpen() const
    {
        if (!_acceptor.has_value())
        {
            return false;
        }

        return _acceptor->is_open();
    }

    auto Server::GetExecutor() -> execution::AsioExecutor&
    {
        return _executor;
    }

    auto Server::GetExecutor() const -> const execution::AsioExecutor&
    {
        return _executor;
    }

    auto Server::GetListenPort() const -> uint16_t
    {
        return _listenPort;
    }

    auto Server::GetSessionCount() const -> int64_t
    {
        return _sessionCount.load();
    }

    auto Server::GetEventChannel() -> const std::shared_ptr<server::event_channel_type>&
    {
        assert(_externalChannel);
        return _externalChannel;
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

            HandleAcceptError(ec);
        }
        else
        {
            HandleAccept(std::move(socket));
        }

        AcceptAsync();
    }

    void Server::HandleAccept(boost::asio::ip::tcp::socket socket)
    {
        session::id_type id = PublishSessionId();
        auto channel = std::make_shared<session::event_channel_type>();
        auto strand = std::make_shared<execution::AsioStrand>(make_strand(_executor.GetIoContext()));
        auto session = std::make_shared<Session>(id, channel, std::move(socket), strand);

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

        ++_sessionCount;
        session->StartReceive();

        Post(*strand, [this, channel = std::move(channel), strand]() mutable
            {
                Run(std::move(channel), std::move(strand));
            });

        _externalChannel->Send(server::ConnectionEvent{ .id = id, .session = std::move(session) },
            channel::ChannelSignal::NotifyOne);
    }

    void Server::HandleAcceptError(const boost::system::error_code& ec)
    {
        (void)ec;
    }

    auto Server::Run(
        std::shared_ptr<session::event_channel_type> channel,
        SharedPtrNotNull<execution::AsioStrand> strand) -> Future<void>
    {
        (void)strand;

        const auto holder = shared_from_this();
        (void)holder;

        AsyncEnumerable<session::event_type> enumerable(std::move(channel));
        assert(ExecutionContext::GetExecutor() == strand.get());

        while (enumerable.HasNext())
        {
            try
            {
                HandleSessionEvent(co_await enumerable);
            }
            catch (...)
            {
                co_return;
            }
        }
    }

    void Server::HandleSessionEvent(session::event_type event)
    {
        std::visit([this]<typename T>(T&& arg)
        {
            if constexpr (std::is_same_v<T, session::ReceiveEvent>)
            {
                this->HandleSessionReceive(std::forward<T>(arg));
            }
            else if constexpr (std::is_same_v<T, session::IoErrorEvent>)
            {
                this->HandleSessionError(std::forward<T>(arg));
            }
            else if constexpr (std::is_same_v<T, session::DestructEvent>)
            {
                this->HandleSessionDestruct(std::forward<T>(arg));
            }
            else
            {
                static_assert(!sizeof(T), "not implemented");
            }
        }, std::move(event));
    }

    void Server::HandleSessionReceive(session::ReceiveEvent event)
    {
        server::ReceiveEvent serverEvent{
            .id = event.id,
            .buffer = std::move(event.buffer),
        };

        _externalChannel->Send(std::move(serverEvent), channel::ChannelSignal::NotifyOne);
    }

    void Server::HandleSessionError(session::IoErrorEvent event)
    {
        // log
        bool erased = false;
        {
            std::lock_guard lock(_sessionMutex);
            erased = _sessions.erase(event.id);
        }

        if (erased)
        {
            _externalChannel->Send(server::DisconnectionEvent{ .id = event.id, },
                channel::ChannelSignal::NotifyOne);
        }
    }

    void Server::HandleSessionDestruct(session::DestructEvent event)
    {
        _sessionIdRecycleQueue.push(event.id.Unwrap());
        --_sessionCount;
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
