#pragma once
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <boost/asio/ip/tcp.hpp>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_hash_map.h>
#include "zerosugar/shared/log/log_service_interface.h"
#include "zerosugar/shared/service/service_locator.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/network/session/event.h"

namespace zerosugar
{
    class Session;

    namespace execution
    {
        class AsioExecutor;
        class AsioStrand;
    }

    class Server : public std::enable_shared_from_this<Server>
    {
    public:
        using Locator = ServiceLocatorRef<ILogService>;

    public:
        Server(std::string name, Locator locator, execution::AsioExecutor& executor);

        virtual bool StartUp(uint16_t listenPort);
        virtual void Shutdown();

        bool IsOpen() const;

        auto GetName() const -> const std::string&;
        auto GetExecutor() -> execution::AsioExecutor&;
        auto GetExecutor() const -> const execution::AsioExecutor&;
        auto GetListenPort() const -> uint16_t;
        auto GetSessionCount() const -> int64_t;

    private:
        void AcceptAsync();
        void OnAccept(const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket);

    private:
        void HandleAccept(boost::asio::ip::tcp::socket socket);
        void HandleAcceptError(const boost::system::error_code& ec);

        auto Run(SharedPtrNotNull<session::event_channel_type> channel,
            SharedPtrNotNull<execution::AsioStrand> strand) -> Future<void>;
        void HandleSessionEvent(session::event_type event);
        void HandleSessionReceive(session::ReceiveEvent event);
        void HandleSessionError(session::IoErrorEvent event);
        void HandleSessionDestruct(session::DestructEvent event);

    private:
        virtual void OnAccept(Session& session) = 0;
        virtual void OnReceive(Session& session, Buffer buffer) = 0;
        virtual void OnError(Session& session, const boost::system::error_code& error) = 0;

    private:
        auto FindSession(session::id_type id) const -> std::shared_ptr<Session>;
        auto PublishSessionId() -> session::id_type;

    private:
        std::string _name;
        Locator _locator;
        execution::AsioExecutor& _executor;
        uint16_t _listenPort = 0;
        std::optional<boost::asio::ip::tcp::acceptor> _acceptor = std::nullopt;

        std::atomic<int64_t> _sessionCount = 0;
        std::atomic<session::id_type::value_type> _nextSessionId = session::id_type::Default().Unwrap();
        tbb::concurrent_queue<session::id_type::value_type> _sessionIdRecycleQueue;
        mutable std::mutex _sessionMutex;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<Session>> _sessions;
    };
}
