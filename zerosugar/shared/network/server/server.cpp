#include "server.h"

#include <cassert>

#include "zerosugar/shared/execution/channel/async_enumerable.h"
#include "zerosugar/shared/execution/future/future.hpp"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/execution/executor/operation/post.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/shared/service/service_locator_log.h"

namespace zerosugar
{
    Server::Server(std::string name, execution::AsioExecutor& executor)
        : _name(std::format("{}_server", std::move(name)))
        , _executor(executor)
    {
    }

    void Server::Initialize(ServiceLocator& dependencyLocator)
    {
        _locator = dependencyLocator;
    }

    void Server::StartUp(uint16_t listenPort)
    {
        if (_acceptor.has_value())
        {
            assert(false);
            return;
        }

        _listenPort = listenPort;

        try
        {
            constexpr bool reuseAddress = false;
            _acceptor.emplace(_executor.GetIoContext(),
                boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), _listenPort),
                reuseAddress);
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_CRITICAL(_locator, std::format("[{}] fail to start. exception: {}",
                GetName(), e.what()));
        }

        if (!_acceptor.has_value() || !_acceptor->is_open())
        {
            return;
        }

        AcceptAsync();
    }

    void Server::Shutdown()
    {
        if (!_acceptor.has_value())
        {
            assert(false);
            return;
        }

        boost::system::error_code ec;
        _acceptor->cancel(ec);

        {
            std::lock_guard lock(_sessionMutex);
            for (const auto& session : _sessions | std::views::values)
            {
                session->Close();
            }
        }

        if (ec)
        {
            ZEROSUGAR_LOG_CRITICAL(_locator, std::format("[{}] has a error on shutdown process. errorCode: [{}, {}]",
                GetName(), ec.value(), ec.message()));
        }
    }

    bool Server::IsOpen() const
    {
        if (!_acceptor.has_value())
        {
            return false;
        }

        return _acceptor->is_open();
    }

    auto Server::GetName() const -> const std::string&
    {
        return _name;
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

        this->OnAccept(*session);
        session->StartReceive();

        Post(*strand, [this, channel = std::move(channel), strand]() mutable
            {
                Run(std::move(channel), std::move(strand));
            });
    }

    void Server::HandleAcceptError(const boost::system::error_code& ec)
    {
        if (ec)
        {
            ZEROSUGAR_LOG_CRITICAL(_locator, std::format("[{}] fail to accept session. errorCode: [{}, {}]",
                GetName(), ec.value(), ec.message()));
        }
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
            catch (const std::exception& e)
            {
                ZEROSUGAR_LOG_WARN(_locator, std::format("[{}] session event handler throw an exception. exception: {}",
                    GetName(), e.what()));
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
        std::shared_ptr<Session> session = FindSession(event.id);
        if (!session)
        {
            return;
        }

        this->OnReceive(*session, std::move(event.buffer));
    }

    void Server::HandleSessionError(session::IoErrorEvent event)
    {
        std::shared_ptr<Session> session;

        decltype(_sessions)::accessor accessor;
        if (_sessions.find(accessor, event.id))
        {
            session = std::move(accessor->second);
            _sessions.erase(accessor);
        }

        if (!session)
        {
            return;
        }

        this->OnError(*session, event.errorCode);
    }

    void Server::HandleSessionDestruct(session::DestructEvent event)
    {
        _sessionIdRecycleQueue.push(event.id.Unwrap());
        --_sessionCount;
    }

    auto Server::FindSession(session::id_type id) const -> std::shared_ptr<Session>
    {
        decltype(_sessions)::const_accessor accessor;
        if (_sessions.find(accessor, id))
        {
            return accessor->second;
        }

        return nullptr;
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
