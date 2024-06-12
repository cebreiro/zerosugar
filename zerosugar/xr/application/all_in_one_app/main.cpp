#include "zerosugar/xr/application/all_in_one_app/all_in_one_app.h"
#include "zerosugar/xr/application/all_in_one_app/all_in_one_app_config.h"

int main(int argc, char* argv[])
{
    using namespace zerosugar::xr;

    // TODO: remove hard code
    AllInOneApp server(AllInOneAppConfig{
        .workerCount = std::thread::hardware_concurrency(),
        });

    return server.Run(std::span(argv, argc));
}
