#pragma once
#include "zerosugar/shared/network/server/server.h"

namespace zerosugar::execution
{
    class AsioExecutor;
}

namespace zerosugar::xr
{
    class LoginServer : public Server
    {
    public:
        explicit LoginServer(SharedPtrNotNull<execution::AsioExecutor> executor);

    private:
    };
}
