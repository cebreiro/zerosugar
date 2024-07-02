#include "zerosugar/xr/application/bot_client/bot_client_app.h"
#include "zerosugar/xr/application/bot_client/bot_client_app_config.h"

int main(int argc, char* argv[])
{
    using namespace zerosugar::xr;

    BotClientApp server;

    return server.Run(std::span(argv, argc));
}
