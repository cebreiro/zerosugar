#include "zerosugar/sl/executable/monolithic_server/monolithic_server.h"

int main(int argc, char* argv[])
{
    zerosugar::sl::MonolithicServerApplication app;

    return app.Run(std::span(argv, argc));
}
