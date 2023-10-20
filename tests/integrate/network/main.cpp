#include <locale>
#include <random>
#include <vector>

#include "tests/integrate/network/test_app.h"
#include "tests/integrate/network/client.h"
#include "tests/integrate/network/server_event_handler.h"
#include "zerosugar/shared/execution/future/operation/wait_all.h"


TEST(IntegarteNetwork, EchoCompareBytes)
{
    // arrange
    TestApp app;
    std::jthread thread([&app]()
        {
            app.Run();
        });
    while (!app.IsRunning())
    {
    }

    Server& server = app.GetServer();
    ASSERT_TRUE(server.IsOpen());

    auto serverEventHandler = std::make_shared<ServerEventHandler>(app);

    std::mt19937 mt(std::random_device{}());
    std::vector<std::shared_ptr<Client>> clients;

    std::vector<Future<boost::system::error_code>> results;
    results.reserve(clientCount);

    // act
    Future<bool> serverOperation = serverEventHandler->Run();

    for (int64_t i = 0; i < clientCount; ++i)
    {
        Promise<boost::system::error_code> promise;
        results.push_back(promise.GetFuture());

        Client& client = *clients.emplace_back(std::make_shared<Client>(
            server.GetExecutor(), std::move(promise), app.PORT, mt));
        client.Start();
    }

    WaitAll(app.GetExecutor(), results).Wait();

    server.Shutdown();
    const bool serverOperationResult = serverOperation.Get();

    // assert
    ASSERT_TRUE(serverOperationResult);

    for (Future<boost::system::error_code>& clientOperation : results)
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

        ASSERT_EQ(sendBufferSize, receiveBufferSize);

        for (int64_t i = 0; i < std::min(sendBufferSize, receiveBufferSize); ++i)
        {
            EXPECT_EQ(sendBuffer[i], receiveBuffer[i]) << std::format("diff i={}", i);
        }
    }

    // finalize
    app.Shutdown();
}

int main()
{
    std::locale::global(std::locale("kor"));
     
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
