#include <google/protobuf/compiler/plugin.h>
#include "zerosugar/tool/proto_code_generator/proto_code_generator.h"


int main(int argc, char* argv[])
{
    zerosugar::ProtoCodeGenerator generator;
    return PluginMain(argc, argv, &generator);
}
