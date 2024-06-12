#include "session.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"

namespace zerosugar
{
    Session::Session(id_type id, SharedPtrNotNull<session::event_channel_type> channel,
        boost::asio::ip::tcp::socket socket, SharedPtrNotNull<execution::AsioStrand> strand)
        : _id(id)
        , _channel(std::move(channel))
        , _socket(std::move(socket))
        , _strand(std::move(strand))
        , _localAddress(_socket.local_endpoint().address().to_string())
        , _localPort(_socket.local_endpoint().port())
        , _remoteAddress(_socket.remote_endpoint().address().to_string())
        , _remotePort(_socket.remote_endpoint().port())
    {
        assert(_socket.is_open());

        ExpandReceiveBuffer();
    }

    Session::~Session()
    {
        session::DestructEvent event{
            .id = _id,
        };

        _channel->Send(std::move(event), channel::ChannelSignal::NotifyOne);
    }

    void Session::StartReceive()
    {
        post(_strand->GetImpl(), [self = shared_from_this()]()
            {
                self->ReceiveAsync();
            });
    }

    void Session::Send(Buffer buffer)
    {
        dispatch(_strand->GetImpl(), [self = shared_from_this(), buffer = std::move(buffer)]() mutable
            {
                self->SendAsync(std::move(buffer));
            });
    }

    void Session::Close()
    {
        dispatch(_strand->GetImpl(), [self = shared_from_this()]()
            {
                boost::system::error_code ec;
                self->_socket.close(ec);
                self->_channel->Close();
            });
    }

    bool Session::IsOpen() const
    {
        return _socket.is_open();
    }

    auto Session::GetId() const -> id_type
    {
        return _id;
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

    auto Session::GetStrand() -> execution::AsioStrand&
    {
        return *_strand;
    }

    auto Session::GetStrand() const -> const execution::AsioStrand&
    {
        return *_strand;
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

        _socket.async_read_some(_mutableBuffers, bind_executor(_strand->GetImpl(),
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

    void Session::OnWriteAsync(int64_t bytes)
    {
        assert(_sendBuffer.has_value());
        assert(!_constBuffers.empty());

        _constBuffers.clear();
        if (!_sendBuffer.has_value())
        {
            assert(false);
            Close();

            return;
        }

        Buffer buffer;
        if (!_sendBuffer->SliceFront(buffer, bytes))
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

        _channel->Send(event, channel::ChannelSignal::NotifyOne);
    }

    void Session::SendReceiveEvent(Buffer buffer)
    {
        session::ReceiveEvent event{
            .id = _id,
            .buffer = std::move(buffer),
        };

        _channel->Send(std::move(event), channel::ChannelSignal::NotifyOne);
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
        async_write(_socket, constBuffer, bind_executor(_strand->GetImpl(),
            [self = shared_from_this()](const boost::system::error_code& ec, size_t bytes)
            {
                if (ec)
                {
                    self->HandleError(ec);
                    return;
                }

                self->OnWriteAsync(static_cast<int64_t>(bytes));
            }));
    }
}
