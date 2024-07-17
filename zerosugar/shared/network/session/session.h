#pragma once
#include <memory>
#include <fmt/core.h>
#include <fmt/format.h>
#include <boost/asio.hpp>
#include "zerosugar/shared/network/session/event.h"
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar
{
    class ILogService;
}

namespace zerosugar
{
    namespace execution
    {
        class AsioStrand;
    }

    class Session : public std::enable_shared_from_this<Session>
    {
    public:
        using id_type = session::id_type;
        static constexpr int64_t RECEIVE_BUFFER_MIN_SIZE = 1024;
        static constexpr int64_t RECEIVE_BUFFER_EXPAND_SIZE = 2048;

    public:
        Session(const Session& other) = delete;
        Session(Session&& other) noexcept = delete;
        Session& operator=(const Session& other) = delete;
        Session& operator=(Session&& other) noexcept = delete;

        Session(id_type id,
            SharedPtrNotNull<session::event_channel_type> channel,
            boost::asio::ip::tcp::socket socket,
            SharedPtrNotNull<execution::AsioStrand> strand,
            std::shared_ptr<ILogService> logService);
        ~Session();

        void StartReceive();
        void Send(Buffer buffer);
        void Close();

        bool IsOpen() const;

        auto GetId() const -> id_type;
        auto GetLocalAddress() const -> const std::string&;
        auto GetLocalPort() const -> uint16_t;

        auto GetRemoteAddress() const -> const std::string&;
        auto GetRemotePort() const -> uint16_t;

        auto GetStrand() -> execution::AsioStrand&;
        auto GetStrand() const -> const execution::AsioStrand&;

    private:
        void ReceiveAsync();
        void HandleReceive(int64_t bytes);

        void SendAsync(Buffer buffer);
        void OnWriteAsync(int64_t bytes);

        void HandleError(const boost::system::error_code& ec);

    private:
        void SendReceiveEvent(Buffer buffer);

        void ExpandReceiveBuffer();
        auto FlushSendWaitQueue() -> Buffer;
        auto ConfigureConstBuffer(Buffer& buffer) -> const std::vector<boost::asio::const_buffer>&;
        void WriteAsync(const std::vector<boost::asio::const_buffer>& constBuffer);

    private:
        id_type _id = id_type::Default();
        SharedPtrNotNull<session::event_channel_type> _channel;
        boost::asio::ip::tcp::socket _socket;
        SharedPtrNotNull<execution::AsioStrand> _strand;
        std::shared_ptr<ILogService> _logService;

        std::string _localAddress;
        uint16_t _localPort = 0;
        std::string _remoteAddress;
        uint16_t _remotePort = 0;

        Buffer _receiveBuffer;
        std::vector<boost::asio::mutable_buffer> _mutableBuffers;

        bool _sendPending = false;
        Buffer _sendBuffer;
        std::vector<Buffer> _sendWaitQueue;
        std::vector<boost::asio::const_buffer> _constBuffers;
    };
}

namespace fmt
{
    template <>
    struct formatter<zerosugar::Session> : formatter<std::string>
    {
        auto format(const zerosugar::Session& session, format_context& ctx) const
        {
            return formatter<std::string>::format(
                fmt::format("{{ id: \"{}\", address: \"{}:{}\" }}",
                    session.GetId(), session.GetRemoteAddress(), session.GetRemotePort()), ctx);
        }
    };
}
