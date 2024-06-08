#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <optional>

namespace zerosugar::sl
{
    struct EnumValue
    {
        std::string name;
        int32_t number = -1;
    };

    struct Enum
    {
        std::string name;
        std::vector<EnumValue> values;
    };

    struct FieldOption
    {
        std::optional<int32_t> length = std::nullopt;
        std::optional<std::string> sizeElement = std::nullopt;
    };

    struct Field
    {
        using map_type = std::optional<std::pair<std::string, std::string>>;

        std::string name;
        std::string type;
        bool optional = false;
        bool repeated = false;
        bool required = false;
        map_type map = std::nullopt;
        FieldOption option;
    };

    struct MessageOption
    {
        std::optional<int32_t> opcode = std::nullopt;
    };

    struct Message
    {
        std::string name;
        std::vector<Field> fields;
        MessageOption option;
    };

    struct Method
    {
        using inout_type = std::optional<std::pair<std::string, bool>>;

        std::string name;
        inout_type input = std::nullopt;
        inout_type output = std::nullopt;
    };

    struct Service
    {
        std::string name;
        std::vector<Method> methods;
    };

    struct Import
    {
        std::string name;
    };

    struct WriterInput
    {
        std::string package;
        std::vector<Enum> enums;
        std::vector<Message> messages;
        std::vector<Service> services;
        std::vector<Import> imports;
    };
}
