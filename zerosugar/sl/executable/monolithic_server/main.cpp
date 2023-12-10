#include "zerosugar/sl/executable/monolithic_server/application.h"

int main(int argc, char* argv[])
{
    zerosugar::sl::ServerApplication app;

    return app.Run(std::span(argv, argc));
}
