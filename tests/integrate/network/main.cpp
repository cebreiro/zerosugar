#include <locale>

#include <random>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_hash_map.h>
#include <boost/container/static_vector.hpp>

#include "zerosugar/shared/app/app.h"
#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/shared/execution/channel/async_enumerable.h"
#include "zerosugar/shared/execution/executor/static_thread_pool.h"
#include "zerosugar/shared/execution/executor/strand.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/operation/post.h"
#include "zerosugar/shared/execution/future/future.hpp"

using zerosugar::App;
using zerosugar::AppInstance;
using zerosugar::IService;
using zerosugar::ServiceLocator;
using zerosugar::Server;
using zerosugar::Session;
using zerosugar::session::id_type;
using zerosugar::Buffer;
using zerosugar::Promise;
using zerosugar::Future;
using zerosugar::AsyncEnumerable;
using zerosugar::SharedPtrNotNull;
using zerosugar::Strand;
using zerosugar::execution::IExecutor;
using zerosugar::execution::AsioExecutor;

class TestContextService : public AppInstance, public IService, public std::enable_shared_from_this<TestContextService>
{
public:
    static constexpr uint16_t PORT = 52342; // magic number

public:
    TestContextService()
        : AppInstance()
        , _server(std::make_shared<Server>(GetAsioExecutor()))
    {
    }

    auto GetServer() -> Server&
    {
        return *_server;
    }

private:
    void OnStartUp(ServiceLocator& serviceLocator) override
    {
        if (!_server->StartUp(PORT))
        {
            throw std::runtime_error("fail to start server");
        }

        serviceLocator.Add<TestContextService>(shared_from_this());
    }

    void OnShutdown() override
    {
        _server->Shutdown();
    }

private:
    SharedPtrNotNull<Server> _server;
};

constexpr int64_t clientCount = 50;
constexpr int64_t byteSize = 1000000;

class Client : public std::enable_shared_from_this<Client>
{
public:
    Client() = default;
    Client(AsioExecutor& executor, Promise<boost::system::error_code> promise, uint16_t port, std::mt19937& mt)
        : _executor(&executor)
        , _strand(make_strand(executor.GetIoContext()))
        , _promise(std::move(promise))
        , _port(port)
    {
        std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());

        for (int64_t i = 0; i < byteSize; ++i)
        {
            _bytes.emplace_back(static_cast<char>(dist(mt) % std::numeric_limits<uint8_t>::max()));
        }
    }

    void Start()
    {
        assert(!_socket.has_value());
        _socket = boost::asio::ip::tcp::socket(_executor->GetIoContext());

        ConnectAsync();
    }

    auto GetSendBuffer() const -> const auto&
    {
        return _bytes;
    }

    auto GetReceiveBuffer() const -> const auto&
    {
        return _result;
    }

private:
    void Complete(boost::system::error_code ec)
    {
        bool expected = false;
        if (_complete.compare_exchange_strong(expected, true))
        {
            dispatch(_strand, [self = shared_from_this(), this, ec]() mutable
                {
                    self->_socket->close();
                    self->_socket.reset();

                    self->_promise.Set(std::move(ec));
                });
        }
    }

    void ConnectAsync()
    {
        using namespace boost::asio::ip;

        _socket->async_connect(tcp::endpoint(make_address("127.0.0.1"), _port),
            [self = shared_from_this()](const boost::system::error_code& ec)
            {
                self->OnConnect(ec);
            });
    }

    void OnConnect(const boost::system::error_code& ec)
    {
        if (ec)
        {
            ConnectAsync();

            return;
        }

        ReceiveAsync();
        SendAsync();
    }

    void ReceiveAsync()
    {
        assert(byteSize - _receiveSize > 0);

        _socket->async_read_some(boost::asio::buffer(_result.data() + _receiveSize, byteSize - _receiveSize),
            bind_executor(_strand, [self = shared_from_this(), this](const boost::system::error_code& ec, size_t n)
                {
                    if (ec)
                    {
                        Complete(ec);

                        return;
                    }

                    _receiveSize += static_cast<int64_t>(n);
                    if (_receiveSize < byteSize)
                    {
                        ReceiveAsync();
                    }
                    else if (_receiveSize == byteSize)
                    {
                        Complete(boost::system::error_code());
                    }
                    else
                    {
                        assert(false);
                    }
                }));
    }

    void SendAsync()
    {
        assert(std::ssize(_bytes) - _sendSize > 0);

        async_write(*_socket, boost::asio::buffer(_bytes.data() + _sendSize, std::ssize(_bytes) - _sendSize),
            bind_executor(_strand, [self = shared_from_this(), this](const boost::system::error_code& ec, size_t n)
            {
                if (ec)
                {
                    Complete(ec);

                    return;
                }

                _sendSize += static_cast<int64_t>(n);
                if (_sendSize == byteSize)
                {
                    return;
                }
                else
                {
                    // is it possible?
                    assert(false);
                    SendAsync();
                }
            }));
    }

    auto GetResultBuffer() const -> const std::array<char, byteSize>&
    {
        return _result;
    }

private:
    std::atomic<bool> _complete = false;
    AsioExecutor* _executor = nullptr;
    boost::asio::strand<boost::asio::io_context::executor_type> _strand;
    Promise<boost::system::error_code> _promise;
    uint16_t _port = 0;
    std::optional<boost::asio::ip::tcp::socket> _socket = std::nullopt;
    boost::container::static_vector<char, byteSize> _bytes;
    int64_t _sendSize = 0;
    std::array<char, byteSize> _result = {};
    int64_t _receiveSize = 0;
};

struct ServerContext
{
    ServerContext(id_type id, SharedPtrNotNull<Session> session)
        : id(id)
        , session(std::move(session))
    {
    }

    id_type id = id_type::Default();
    SharedPtrNotNull<Session> session;
};

TEST(IntegarteNetwork, EchoCompareBytes)
{
    // arrange
    auto app = std::make_shared<TestContextService>();
    std::jthread thread([app]()
        {
            app->Run();
        });
    while (!app->IsRunning())
    {
    }

    auto context = App::GetServiceLocator().Find<TestContextService>();

    auto& threadPool = zerosugar::StaticThreadPool::GetInstance();
    const int64_t concurrency = threadPool.GetConcurrency();

    IExecutor& executor = threadPool;
    std::vector<std::shared_ptr<Strand>> strands;

    for (int64_t i = 0; i < concurrency; ++i)
    {
        strands.emplace_back(std::make_shared<Strand>(executor.SharedFromThis()));
    }

    Server& server = context->GetServer();
    ASSERT_TRUE(server.IsOpen());

    auto channel = server.GetEventChannel();
    Future<Future<bool>> serverOperation;
    tbb::concurrent_hash_map<id_type, std::shared_ptr<ServerContext>> serverContextTable;

    std::mt19937 mt(std::random_device{}());
    boost::container::static_vector<std::shared_ptr<Client>, clientCount> clients;
    std::vector<Future<boost::system::error_code>> clientOperations;
    clientOperations.reserve(clientCount);

    // act
    serverOperation = StartAsync(executor,
        [](decltype(channel) channel, std::vector<std::shared_ptr<Strand>>& strands, decltype(serverContextTable)& table) -> Future<bool>
        {
            using namespace zerosugar::server;

            AsyncEnumerable<event_type> enumerable(std::move(channel));

            while (enumerable.HasNext())
            {
                try
                {
                    event_type event = co_await enumerable;

                    if (auto e = std::get_if<ConnectionEvent>(&event); e != nullptr)
                    {
                        auto client = std::make_shared<ServerContext>(e->id, e->session);

                        decltype(serverContextTable)::accessor accessor;
                        if (table.insert(accessor, e->session->GetId()))
                        {
                            accessor->second = std::move(client);
                        }
                        else
                        {
                            co_return false;
                        }
                    }
                    if (auto e = std::get_if<DisconnectionEvent>(&event); e != nullptr)
                    {
                        table.erase(e->id);
                    }
                    if (auto e = std::get_if<ReceiveEvent>(&event); e != nullptr)
                    {
                        std::shared_ptr<ServerContext> serverContext;

                        decltype(serverContextTable)::const_accessor accessor;
                        if (table.find(accessor, e->id))
                        {
                            serverContext = accessor->second;
                        }
                        else
                        {
                            co_return false;
                        }

                        IExecutor& strand = *strands[e->id.Unwrap() % strands.size()];
                        Dispatch(strand, [event = std::move(*e), serverContext]() mutable
                            {
                                serverContext->session->Send(std::move(event.buffer));
                            });
                    }
                }
                catch (...)
                {
                    co_return true;
                }
            }

            co_return true;
        }, channel, std::ref(strands), std::ref(serverContextTable));

    for (int64_t i = 0; i < clientCount; ++i)
    {
        Promise<boost::system::error_code> promise;
        clientOperations.push_back(promise.GetFuture());

        Client& client = *clients.emplace_back(std::make_shared<Client>(server.GetExecutor(), std::move(promise), context->PORT, mt));
        client.Start();
    }

    WaitAll(executor, clientOperations).Wait();

    server.Shutdown();
    const bool serverOperationResult = serverOperation.Get().Get();

    // assert
    ASSERT_TRUE(serverOperationResult);

    for (Future<boost::system::error_code>& clientOperation : clientOperations)
    {
        boost::system::error_code ec = clientOperation.Get();
        if (ec)
        {
            ASSERT_TRUE(false) << ec.message();
        }
    }

    EXPECT_EQ(std::ssize(clients), clientCount);

    for (const Client& client : clients | zerosugar::notnull::reference)
    {
        const auto& sendBuffer = client.GetSendBuffer();
        const auto& receiveBuffer = client.GetReceiveBuffer();

        const int64_t sendBufferSize = std::ssize(sendBuffer);
        const int64_t receiveBufferSize = std::ssize(receiveBuffer);

        EXPECT_EQ(sendBufferSize, receiveBufferSize);

        for (int64_t i = 0; i < std::min(sendBufferSize, receiveBufferSize); ++i)
        {
            EXPECT_EQ(sendBuffer[i], receiveBuffer[i]) << std::format("diff i={}", i);
        }
    }

    // finalize
    app->Shutdown();
}


int main()
{
    std::locale::global(std::locale("kor"));
     
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
