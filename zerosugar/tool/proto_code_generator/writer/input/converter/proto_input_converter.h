#pragma once
#include "zerosugar/tool/proto_code_generator/writer/input/writer_input.h"

namespace google::protobuf
{
    class FileDescriptor;
}

namespace zerosugar
{
    class WriterInputProtoConverter
    {
    public:
        WriterInputProtoConverter() = default;

        auto Convert(const google::protobuf::FileDescriptor& fileDescriptor) const -> WriterInput;
    };
}
