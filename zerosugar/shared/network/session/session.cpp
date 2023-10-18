#include "session.h"

namespace zerosugar
{
    Session::Session(id_type id, SharedPtrNotNull<session::event_channel_type> channel,
        boost::asio::ip::tcp::socket socket, boost::asio::strand<boost::asio::io_context::executor_type> strand)
        : _id(id)
        , _eventChannel(std::move(channel))
        , _socket(std::move(socket))
        , _strand(std::move(strand))
        , _localAddress(_socket.local_endpoint().address().to_string())
        , _localPort(_socket.local_endpoint().port())
        , _remoteAddress(_socket.remote_endpoint().address().to_string())
        , _remotePort(_socket.remote_endpoint().port())
    {
    }

    Session::~Session()
    {
        session::DestructEvent event{
            .id = _id,
            .remoteAddress = _remoteAddress,
            .port = _remotePort,
        };

        _eventChannel->Send(std::move(event), channel::ChannelSignal::NotifyAll);
    }

    void Session::StartReceive()
    {
        post(_strand, [self = shared_from_this()]()
            {
                self->ReceiveAsync();
            });
    }

    void Session::Send(Buffer buffer)
    {
        post(_strand, [self = shared_from_this(), buffer = std::move(buffer)]() mutable
            {
                self->SendAsync(std::move(buffer));
            });
    }

    void Session::Close()
    {
        post(_strand, [self = shared_from_this()]()
            {
                boost::system::error_code ec;
                self->_socket.close(ec);
            });
    }

    auto Session::GetLocalAddress() const -> const std::string&
    {
        return _localAddress;
    }

    auto Session::GetLocalPort() const -> uint16_t
    {
        return _localPort;
    }

    auto Session::GetRemoteAddress() const -> const std::string&
    {
        return _remoteAddress;
    }

    auto Session::GetRemotePort() const -> uint16_t
    {
        return _remotePort;
    }

    void Session::ReceiveAsync()
    {
        assert(_mutableBuffers.empty());
        assert(_receiveBuffers.GetSize() >= RECEIVE_BUFFER_MIN_SIZE);

        for (buffer::Fragment& fragment : _receiveBuffers.GetFragmentContainer())
        {
            assert(fragment.IsValid());

            _mutableBuffers.emplace_back(fragment.GetData(), fragment.GetSize());
        }

        _socket.async_read_some(_mutableBuffers, bind_executor(_strand,
            [self = shared_from_this()](const boost::system::error_code& ec, size_t bytes)
            {
                if (ec)
                {
                    self->HandleError(ec);
                    return;
                }

                self->HandleReceive(static_cast<int64_t>(bytes));
            }));
    }

    void Session::HandleReceive(int64_t bytes)
    {
        _mutableBuffers.clear();

        Buffer buffer;
        if (!_receiveBuffers.SliceFront(buffer, bytes))
        {
            assert(false);
            Close();

            return;
        }

        ExpandReceiveBuffer();
        SendReceiveEvent(std::move(buffer));
        ReceiveAsync();
    }

    void Session::SendAsync(Buffer buffer)
    {
        assert(buffer.GetSize() > 0);

        if (_sendBuffer.has_value())
        {
            assert(!_constBuffers.empty());

            _sendWaitQueue.push_back(std::move(buffer));
            return;
        }

        assert(_sendWaitQueue.empty());

        Buffer& sendBuffer = _sendBuffer.emplace();
        sendBuffer.MergeBack(std::move(buffer));

        WriteAsync(ConfigureConstBuffer(sendBuffer));
    }

    void Session::HandleSend(int64_t bytes)
    {
        assert(_sendBuffer.has_value());
        assert(!_constBuffers.empty());

        _constBuffers.clear();

        Buffer buffer;
        if (!_sendBuffer.has_value() || _sendBuffer->SliceFront(buffer, bytes))
        {
            assert(false);
            Close();

            return;
        }

        if (_sendBuffer->GetSize() > 0)
        {
            WriteAsync(ConfigureConstBuffer(_sendBuffer.value()));
            return;
        }

        _sendBuffer.reset();

        if (!_sendWaitQueue.empty())
        {
            SendAsync(FlushSendWaitQueue());
        }
    }

    void Session::HandleError(const boost::system::error_code& ec)
    {
        session::IoErrorEvent event{
            .id = _id,
            .errorCode = ec,
        };

        _eventChannel->Send(event, channel::ChannelSignal::NotifyAll);
    }

    void Session::SendReceiveEvent(Buffer buffer)
    {
        session::ReceiveEvent event{
            .id = _id,
            .buffer = std::move(buffer),
        };

        _eventChannel->Send(std::move(event), channel::ChannelSignal::NotifyAll);
    }

    void Session::ExpandReceiveBuffer()
    {
        if (_receiveBuffers.GetSize() >= RECEIVE_BUFFER_MIN_SIZE)
        {
            return;
        }

        auto ptr = std::make_shared<char[]>(RECEIVE_BUFFER_EXPAND_SIZE);

        _receiveBuffers.Add(buffer::Fragment(std::move(ptr), 0, RECEIVE_BUFFER_EXPAND_SIZE));
    }

    auto Session::FlushSendWaitQueue() -> Buffer
    {
        assert(!_sendWaitQueue.empty());

        Buffer sendBuffer;

        for (Buffer& buffer : _sendWaitQueue)
        {
            sendBuffer.MergeBack(std::move(buffer));
        }

        _sendWaitQueue.clear();

        return sendBuffer;
    }

    auto Session::ConfigureConstBuffer(Buffer& buffer) -> const std::vector<boost::asio::const_buffer>&
    {
        assert(buffer.GetSize() > 0);
        assert(_constBuffers.empty());

        for (const buffer::Fragment& fragment : buffer.GetFragmentContainer())
        {
            assert(fragment.IsValid());

            _constBuffers.emplace_back(fragment.GetData(), fragment.GetSize());
        }

        return _constBuffers;
    }

    void Session::WriteAsync(const std::vector<boost::asio::const_buffer>& constBuffer)
    {
        async_write(_socket, constBuffer, bind_executor(_strand,
            [self = shared_from_this()](const boost::system::error_code& ec, size_t bytes)
            {
                if (ec)
                {
                    self->HandleError(ec);
                    return;
                }

                self->HandleSend(static_cast<int64_t>(bytes));
            }));
    }
}
