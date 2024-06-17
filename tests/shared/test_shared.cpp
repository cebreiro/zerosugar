#include <locale>
#include "zerosugar/shared/execution/executor/executor.hpp"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"

int main()
{
    std::locale::global(std::locale("kor"));

    auto executor = std::make_shared<zerosugar::execution::AsioExecutor>(4);
    executor->Run();

    zerosugar::ExecutionContext::PushExecutor(executor.get());

    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
