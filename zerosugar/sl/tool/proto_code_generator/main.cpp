#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <nlohmann/json.hpp>

#include <memory>
#include <format>
#include <Windows.h>

#include "zerosugar/sl/tool/proto_code_generator/proto_code_generator.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/service_interface_writer.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/message_writer.h"
#include "zerosugar/sl/tool/proto_code_generator/writer/input/converter/writer_input_proto_converter.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;



int main(int argc, char* argv[])
{
    zerosugar::sl::ProtoCodeGenerator generator;
    return PluginMain(argc, argv, &generator);
}
