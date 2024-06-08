#pragma once
#include "google/protobuf/descriptor.h"
#include "google/protobuf/descriptor.pb.h"
#include "google/protobuf/wire_format.h"

namespace zerosugar
{
    class ProtoCustomOption
    {
    public:
        ProtoCustomOption(const ProtoCustomOption& other) = delete;
        ProtoCustomOption(ProtoCustomOption&& other) noexcept = delete;
        ProtoCustomOption& operator=(const ProtoCustomOption& other) = delete;
        ProtoCustomOption& operator=(ProtoCustomOption&& other) noexcept = delete;

    private:
        ProtoCustomOption();

    public:
        using message_opcode_option_id_type = google::protobuf::internal::ExtensionIdentifier<
            google::protobuf::MessageOptions,
            google::protobuf::internal::PrimitiveTypeTraits<int32_t>,
            google::protobuf::internal::WireFormatLite::FieldType::TYPE_INT32, false>;

        using field_length_option_id_type = google::protobuf::internal::ExtensionIdentifier<
            google::protobuf::FieldOptions,
            google::protobuf::internal::PrimitiveTypeTraits<int32_t>,
            google::protobuf::internal::WireFormatLite::FieldType::TYPE_INT32, false>;

        using field_size_element_option_id_type = google::protobuf::internal::ExtensionIdentifier<
            google::protobuf::FieldOptions,
            google::protobuf::internal::StringTypeTraits,
            google::protobuf::internal::WireFormatLite::FieldType::TYPE_STRING, false>;

        static auto OpcodeId() -> const message_opcode_option_id_type&;
        static auto LengthId() -> const field_length_option_id_type&;
        static auto SizeElementId() -> const field_size_element_option_id_type&;

    private:
        message_opcode_option_id_type _opcodeId;
        field_length_option_id_type _lengthId;
        field_size_element_option_id_type _sizeElementId;

        static const ProtoCustomOption _instance;
    };
}
