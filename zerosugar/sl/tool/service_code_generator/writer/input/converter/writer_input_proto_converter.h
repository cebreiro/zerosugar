#pragma once
#include "zerosugar/sl/tool/service_code_generator/writer/input/writer_input.h"

namespace google::protobuf
{
    class FileDescriptor;
}

namespace zerosugar::sl
{
    class WriterInputProtoConverter
    {
    public:
        WriterInputProtoConverter() = default;

        auto Convert(const google::protobuf::FileDescriptor& fileDescriptor) const -> WriterInput;
    };
}
