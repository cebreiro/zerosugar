#include "server_event_handler.h"

#include "tests/integrate/network/test_app.h"
#include "zerosugar/shared/execution/channel/async_enumerable.h"
#include "zerosugar/shared/execution/executor/operation/dispatch.h"
#include "zerosugar/shared/execution/future/future_coroutine_traits.h"
#include "zerosugar/shared/execution/future/operation/start_async.h"


using zerosugar::AsyncEnumerable;
using zerosugar::server::event_type;

ServerEventHandler::ServerEventHandler(TestApp& app)
    : _app(app)
{
}

auto ServerEventHandler::Run() -> Future<bool>
{
    return StartAsync(_app.GetExecutor(), [self = shared_from_this()]()
        {
            return self->RunImpl();
        }).Get();
}

auto ServerEventHandler::RunImpl() -> Future<bool>
{
    auto holder = shared_from_this();
    (void)holder;

    AsyncEnumerable<event_type> enumerable(_app.GetServer().GetEventChannel());

    while (enumerable.HasNext())
    {
        try
        {
            event_type e = co_await enumerable;

            if (auto event = std::get_if<ConnectionEvent>(&e); event != nullptr)
            {
                if (!HandleEvent(std::move(*event)))
                {
                    co_return false;
                }
            }
            if (auto event = std::get_if<DisconnectionEvent>(&e); event != nullptr)
            {
                if (!HandleEvent(std::move(*event)))
                {
                    co_return false;
                }
            }
            if (auto event = std::get_if<ReceiveEvent>(&e); event != nullptr)
            {
                if (!HandleEvent(std::move(*event)))
                {
                    co_return false;
                }
            }

        }
        catch (...)
        {
            // operation aborted
            co_return true;
        }
    }
}

bool ServerEventHandler::HandleEvent(ConnectionEvent event)
{
    auto client = std::make_shared<Context>(event.id, event.session);

    decltype(_table)::accessor accessor;
    if (_table.insert(accessor, event.session->GetId()))
    {
        accessor->second = std::move(client);
        return true;
    }

    return false;
}

bool ServerEventHandler::HandleEvent(DisconnectionEvent event)
{
    return _table.erase(event.id);
}

bool ServerEventHandler::HandleEvent(ReceiveEvent event)
{
    std::shared_ptr<Context> context;

    decltype(_table)::const_accessor accessor;
    if (_table.find(accessor, event.id))
    {
        context = accessor->second;
        assert(context);
    }
    else
    {
        return false;
    }

    const auto& strands = _app.GetStrands();
    auto& strand = *strands[event.id.Unwrap() % strands.size()];
    Dispatch(strand, [event = std::move(event), context]() mutable
        {
            context->session->Send(std::move(event.buffer));
        });

    return true;
}
