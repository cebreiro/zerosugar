#pragma once
#include <memory>
#include <boost/asio.hpp>
#include "zerosugar/shared/network/server/event.h"

namespace zerosugar
{
    class Session : public std::enable_shared_from_this<Session>
    {
    public:
        using id_type = session::id_type;
        static constexpr int64_t RECEIVE_BUFFER_MIN_SIZE = 1024;
        static constexpr int64_t RECEIVE_BUFFER_EXPAND_SIZE = 4096;

    public:
        Session(const Session& other) = delete;
        Session(Session&& other) noexcept = delete;
        Session& operator=(const Session& other) = delete;
        Session& operator=(Session&& other) noexcept = delete;

        Session(id_type id,
            SharedPtrNotNull<server::event_channel_type> channel,
            boost::asio::ip::tcp::socket socket,
            boost::asio::strand<boost::asio::io_context::executor_type> strand);
        ~Session();

        void StartReceive();
        void Send(Buffer buffer);
        void Close();

        auto GetLocalAddress() const -> const std::string&;
        auto GetLocalPort() const -> uint16_t;

        auto GetRemoteAddress() const -> const std::string&;
        auto GetRemotePort() const -> uint16_t;

    private:
        void ReceiveAsync();
        void HandleReceive(int64_t bytes);

        void SendAsync(Buffer buffer);
        void HandleSend(int64_t bytes);

        void HandleError(const boost::system::error_code& ec);

    private:
        void SendReceiveEvent(Buffer buffer);

        void ExpandReceiveBuffer();
        auto FlushSendWaitQueue() -> Buffer;
        auto ConfigureConstBuffer(Buffer& buffer) -> const std::vector<boost::asio::const_buffer>&;
        void WriteAsync(const std::vector<boost::asio::const_buffer>& constBuffer);

    private:
        id_type _id = id_type::Default();
        SharedPtrNotNull<server::event_channel_type> _channel;
        boost::asio::ip::tcp::socket _socket;
        boost::asio::strand<boost::asio::io_context::executor_type> _strand;

        std::string _localAddress;
        uint16_t _localPort = 0;
        std::string _remoteAddress;
        uint16_t _remotePort = 0;

        Buffer _receiveBuffers;
        std::vector<boost::asio::mutable_buffer> _mutableBuffers;

        std::optional<Buffer> _sendBuffer;
        std::vector<Buffer> _sendWaitQueue;
        std::vector<boost::asio::const_buffer> _constBuffers;
    };
}
