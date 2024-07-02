#include "zerosugar/xr/application/all_in_one_app/all_in_one_app.h"
#include "zerosugar/xr/application/all_in_one_app/all_in_one_app_config.h"

int main(int argc, char* argv[])
{
    using namespace zerosugar::xr;

    AllInOneApp server;

    return server.Run(std::span(argv, argc));
}
