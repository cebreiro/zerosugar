#include "aio_server.h"
#include "zerosugar/xr/application/aio_server/aio_server.h"
#include "zerosugar/xr/application/aio_server/aio_server_config.h"

int main(int argc, char* argv[])
{
    using namespace zerosugar::xr;

    // TODO: remove hard code
    AIOServer server(AIOServerConfig{
        .workerCount = std::thread::hardware_concurrency(),
        });

    return server.Run(std::span(argv, argc));
}
