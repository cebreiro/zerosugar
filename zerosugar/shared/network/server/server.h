#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <boost/asio/ip/tcp.hpp>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_hash_map.h>
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/network/server/event.h"

namespace zerosugar
{
    class Session;

    namespace execution
    {
        class AsioExecutor;
    }

    class Server : public std::enable_shared_from_this<Server>
    {
    public:
        explicit Server(execution::AsioExecutor& executor);

        bool StartUp(uint16_t listenPort);
        void Shutdown();

        auto GetListenPort() const -> uint16_t;

    private:
        void AcceptAsync();
        void OnAccept(const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket);

        auto Run() -> Future<void>;

        auto PublishSessionId() -> session::id_type;

    private:
        execution::AsioExecutor& _executor;
        uint16_t _listenPort = 0;
        std::optional<boost::asio::ip::tcp::acceptor> _acceptor = std::nullopt;
        std::shared_ptr<server::event_channel_type> _channel;

        std::atomic<session::id_type::value_type> _nextSessionId = session::id_type::Default().Unwrap();
        tbb::concurrent_queue<session::id_type::value_type> _sessionIdRecycleQueue;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<Session>> _sessions;
    };
}
