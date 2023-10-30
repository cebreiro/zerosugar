#pragma once
#include <string>
#include <filesystem>

namespace google::protobuf
{
    class FileDescriptor;
}

namespace zerosugar
{
    namespace generator
    {
        struct Result
        {
            std::filesystem::path headerName;
            std::string header;
            std::string cpp;
        };
    }

    class ICodeGenerator
    {
    public:
        virtual ~ICodeGenerator() = default;

        virtual auto Generate(const google::protobuf::FileDescriptor& fileDescriptor) const -> generator::Result = 0;
    };
}
