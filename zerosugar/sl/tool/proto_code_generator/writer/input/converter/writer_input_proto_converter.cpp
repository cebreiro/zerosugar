#include "writer_input_proto_converter.h"

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <boost/algorithm/string.hpp>
#include <ranges>


using namespace google::protobuf;
using namespace google::protobuf::compiler;

namespace
{
    auto GetEnumRange(const FileDescriptor& fd)
    {
        auto transform = [fd = &fd](int i)
            {
                return fd->enum_type(i);
            };
        auto filter = [](const EnumDescriptor* descriptor)
            {
                return descriptor != nullptr;
            };
        auto toReference = [](const EnumDescriptor* descriptor) -> const EnumDescriptor&
            {
                return *descriptor;
            };

        return std::views::iota(0, fd.enum_type_count())
            | std::views::transform(transform)
            | std::views::filter(filter)
            | std::views::transform(toReference);
    }

    auto GetEnumValueRange(const EnumDescriptor& ed)
    {
        auto transform = [ed = &ed](int i)
            {
                return ed->value(i);
            };
        auto filter = [](const EnumValueDescriptor* descriptor)
            {
                return descriptor != nullptr;
            };
        auto toReference = [](const EnumValueDescriptor* descriptor) -> const EnumValueDescriptor&
            {
                return *descriptor;
            };

        return std::views::iota(0, ed.value_count())
            | std::views::transform(transform)
            | std::views::filter(filter)
            | std::views::transform(toReference);
    }

    auto GetMessageRange(const FileDescriptor& fd)
    {
        auto transform = [fd = &fd](int i)
            {
                return fd->message_type(i);
            };
        auto filter = [](const Descriptor* descriptor)
            {
                return descriptor != nullptr;
            };
        auto toReference = [](const Descriptor* descriptor) -> const Descriptor&
            {
                return *descriptor;
            };

        return std::views::iota(0, fd.message_type_count())
            | std::views::transform(transform)
            | std::views::filter(filter)
            | std::views::transform(toReference);
    }

    auto GetServiceRange(const FileDescriptor& fd)
    {
        auto transform = [fd = &fd](int i)
            {
                return fd->service(i);
            };
        auto filter = [](const ServiceDescriptor* descriptor)
            {
                return descriptor != nullptr;
            };
        auto toReference = [](const ServiceDescriptor* descriptor) -> const ServiceDescriptor&
            {
                return *descriptor;
            };

        return std::views::iota(0, fd.service_count())
            | std::views::transform(transform)
            | std::views::filter(filter)
            | std::views::transform(toReference);
    }

    auto GetMethodRange(const ServiceDescriptor& sd)
    {
        auto transform = [sd = &sd](int i)
            {
                return sd->method(i);
            };
        auto filter = [](const MethodDescriptor* descriptor)
            {
                return descriptor != nullptr;
            };
        auto toReference = [](const MethodDescriptor* descriptor) -> const MethodDescriptor&
            {
                return *descriptor;
            };

        return std::views::iota(0, sd.method_count())
            | std::views::transform(transform)
            | std::views::filter(filter)
            | std::views::transform(toReference);
    }

    auto GetFieldRange(const Descriptor& d)
    {
        auto transform = [d = &d](int i)
            {
                return d->field(i);
            };
        auto filter = [](const FieldDescriptor* descriptor)
            {
                return descriptor != nullptr;
            };
        auto toReference = [](const FieldDescriptor* descriptor) -> const FieldDescriptor&
            {
                return *descriptor;
            };

        return std::views::iota(0, d.field_count())
            | std::views::transform(transform)
            | std::views::filter(filter)
            | std::views::transform(toReference);
    }

    auto GetDependencyRange(const FileDescriptor& fd)
    {
        auto transform = [fd = &fd](int i)
            {
                return fd->dependency(i);
            };
        auto filter = [](const FileDescriptor* descriptor)
            {
                return descriptor != nullptr;
            };
        auto toReference = [](const FileDescriptor* descriptor) -> const FileDescriptor&
            {
                return *descriptor;
            };

        return std::views::iota(0, fd.dependency_count())
            | std::views::transform(transform)
            | std::views::filter(filter)
            | std::views::transform(toReference);
    }

    auto GetPackageNameToNamespace(const std::string& str) -> std::string
    {
        std::string result(str);
        boost::algorithm::replace_all(result, ".", "::");
        boost::algorithm::to_lower(result);

        return result;
    }

    auto ConvertSnakeToCamel(const std::string& str) -> std::string
    {
        std::string result(str);
        for (auto iter = result.begin(); iter != result.end(); )
        {
            if ((*iter) == '_')
            {
                auto next = std::next(iter, 1);
                if (next != result.end())
                {
                    *next = static_cast<char>(::toupper(*next));
                }

                iter = result.erase(iter);
            }
            else
            {
                ++iter;
            }
        }

        return result;
    }

    auto ConvertSnakeToPascal(const std::string& str) -> std::string
    {
        std::string result(str);

        if (!result.empty())
        {
            result[0] = static_cast<char>(::toupper(result[0]));
        }

        return ConvertSnakeToCamel(result);
    }

    auto ConvertProtoTypeToCppType(const std::string& type) -> std::string
    {
        if (type.starts_with("int"))
        {
            return std::format("{}_t", type);
        }

        if (type == "string")
        {
            return std::format("std::{}", type);
        }

        if (type == "bytes")
        {
            return "std::vector<char>";
        }

        return type;
    }
}

namespace zerosugar::sl
{
    struct Factory
    {
        static auto CreateEnumValue(const EnumValueDescriptor& enumValueDescriptor) -> EnumValue
        {
            return EnumValue
            {
                .name = ConvertSnakeToPascal(enumValueDescriptor.name()),
                .number = enumValueDescriptor.number(),
            };
        }

        static auto CreateEnum(const EnumDescriptor& enumDescriptor) -> Enum
        {
            return Enum
            {
                .name = enumDescriptor.name(),
                .values = GetEnumValueRange(enumDescriptor) | std::views::transform(CreateEnumValue) | std::ranges::to<std::vector>(),
            };
        }

        static auto CreateField(const FieldDescriptor& fieldDescriptor) -> Field
        {
            constexpr auto getTypeName = [](const FieldDescriptor& fieldDescriptor) -> std::string
                {
                    if (const EnumDescriptor* enumType = fieldDescriptor.enum_type(); enumType)
                    {
                        return fieldDescriptor.enum_type()->name();
                    }

                    if (const Descriptor* messageType = fieldDescriptor.message_type(); messageType)
                    {
                        return messageType->name();
                    }

                    return ConvertProtoTypeToCppType(fieldDescriptor.type_name());
                };

            return Field
            {
                .name = ConvertSnakeToCamel(fieldDescriptor.name()),
                .type = getTypeName(fieldDescriptor),
                .optional = fieldDescriptor.has_optional_keyword(),
                .repeated = fieldDescriptor.is_repeated(),
                .required = fieldDescriptor.is_required(),
                .map = fieldDescriptor.is_map()
                ? std::make_pair(
                    ConvertProtoTypeToCppType(fieldDescriptor.message_type()->map_key()->type_name()),
                    ConvertProtoTypeToCppType(fieldDescriptor.message_type()->map_value()->type_name()))
                : Field::map_type(std::nullopt),
            };
        }

        static auto CreateMessage(const Descriptor& descriptor) -> Message
        {
            return Message
            {
                .name = descriptor.name(),
                .fields = GetFieldRange(descriptor) | std::views::transform(CreateField) | std::ranges::to<std::vector>(),
            };
        }

        static auto CreateMethod(const MethodDescriptor& methodDescriptor) -> Method
        {
            return Method
            {
                .name = methodDescriptor.name(),
                .input = methodDescriptor.input_type()
                ? Method::inout_type(std::make_pair(methodDescriptor.input_type()->name(), methodDescriptor.client_streaming()))
                : Method::inout_type(std::nullopt),
                .output = methodDescriptor.output_type()
                ? Method::inout_type(std::make_pair(methodDescriptor.output_type()->name(), methodDescriptor.server_streaming()))
                : Method::inout_type(std::nullopt),
            };
        }

        static auto CreateService(const ServiceDescriptor& serviceDescriptor) -> Service
        {
            return Service
            {
                .name = serviceDescriptor.name(),
                .methods = GetMethodRange(serviceDescriptor) | std::views::transform(CreateMethod) | std::ranges::to<std::vector>(),
            };
        }

        static auto CreateImport(const FileDescriptor& fileDescriptor) -> Import
        {
            return Import
            {
                .name = fileDescriptor.name(),
            };
        }
    };

    auto WriterInputProtoConverter::Convert(const FileDescriptor& fileDescriptor) const -> WriterInput
    {
        return WriterInput{
            .package = GetPackageNameToNamespace(fileDescriptor.package()),
            .enums = GetEnumRange(fileDescriptor) | std::views::transform(Factory::CreateEnum) | std::ranges::to<std::vector>(),
            .messages = GetMessageRange(fileDescriptor) | std::views::transform(Factory::CreateMessage) | std::ranges::to<std::vector>(),
            .services = GetServiceRange(fileDescriptor) | std::views::transform(Factory::CreateService) | std::ranges::to<std::vector>(),
            .imports = GetDependencyRange(fileDescriptor) | std::views::transform(Factory::CreateImport) | std::ranges::to<std::vector>(),
        };
    }
}
