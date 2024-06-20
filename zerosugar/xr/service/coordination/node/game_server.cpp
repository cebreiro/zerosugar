#include "game_server.h"

#include "zerosugar/xr/service/coordination/node/game_instance.h"

namespace zerosugar::xr::coordination
{
    GameServer::GameServer(game_server_id_type id, std::string name, std::string ip, uint16_t port)
        : _id(id)
        , _name(std::move(name))
        , _ip(std::move(ip))
        , _port(port)
    {
    }

    void GameServer::Shutdown()
    {
        if (std::exchange(_shutdown, true) == false)
        {
            for (Promise<void>& promise : _responseWaits | std::views::values)
            {
                try
                {
                    throw std::runtime_error("shutdown server");
                }
                catch (...)
                {
                    promise.SetException(std::current_exception());
                }
            }
        }
    }

    auto GameServer::FindGameInstance(int32_t zoneId) -> GameInstance*
    {
        for (GameInstance* instance : GetChildrenRange())
        {
            if (instance->GetZoneId() == zoneId)
            {
                return instance;
            }
        }

        return nullptr;
    }

    void GameServer::MarkCommandResponseAsSuccess(int64_t responseId)
    {
        if (_shutdown)
        {
            return;
        }

        auto iter = _responseWaits.find(responseId);
        if (iter != _responseWaits.end())
        {
            iter->second.Set();

            _responseWaits.erase(iter);
        }
        else
        {
            assert(false);
        }
    }

    void GameServer::MarkCommandResponseAsFailure(int64_t responseId, const std::exception_ptr& exception)
    {
        if (_shutdown)
        {
            return;
        }

        auto iter = _responseWaits.find(responseId);
        if (iter != _responseWaits.end())
        {
            iter->second.SetException(exception);

            _responseWaits.erase(iter);
        }
        else
        {
            assert(false);
        }
    }

    auto GameServer::GetId() const -> const game_server_id_type&
    {
        return _id;
    }

    auto GameServer::GetName() const -> std::string_view
    {
        return _name;
    }

    auto GameServer::GetIP() const -> std::string_view
    {
        return _ip;
    }

    auto GameServer::GetPort() const -> uint16_t
    {
        return _port;
    }

    void GameServer::SetChannel(SharedPtrNotNull<Channel<service::CoordinationCommand>> channel)
    {
        _channel = std::move(channel);
    }

    void GameServer::Send(const service::CoordinationCommand& command)
    {
        assert(_channel);
        assert(!command.responseId.has_value());

        if (_channel->IsOpen())
        {
            _channel->Send(command, channel::ChannelSignal::NotifyOne);
        }
    }

    void GameServer::Send(const service::CoordinationCommand& command, Future<void>& responseCompletionToken)
    {
        assert(!responseCompletionToken.IsValid());
        assert(_channel);
        assert(command.responseId.has_value());
        assert(!_responseWaits.contains(*command.responseId));

        if (_channel->IsOpen())
        {
            Promise<void> promise;
            responseCompletionToken = promise.GetFuture();

            _responseWaits[*command.responseId] = std::move(promise);

            _channel->Send(command, channel::ChannelSignal::NotifyOne);
        }
    }
}
