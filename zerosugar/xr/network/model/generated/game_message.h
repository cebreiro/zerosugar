#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "zerosugar/shared/network/buffer/buffer_deserializable.h"
#include "zerosugar/shared/network/buffer/buffer_serializable.h"

namespace zerosugar::xr::network::game
{
    struct Position : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        float x = {};
        float y = {};
        float z = {};
    };

    struct Rotation : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        float yaw = {};
        float pitch = {};
        float roll = {};
    };

    struct Transform : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        Position position = {};
        Rotation rotation = {};
    };

    struct CharacterStat : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        int32_t hp = {};
        int32_t maxHP = {};
        int32_t attackMin = {};
        int32_t attackMax = {};
        int32_t attackRange = {};
        int32_t attackSpeed = {};
        int32_t defence = {};
        std::string name = {};
        int32_t level = {};
        int32_t gender = {};
        int32_t face = {};
        int32_t hair = {};
        int32_t str = {};
        int32_t dex = {};
        int32_t intell = {};
        float stamina = {};
        float staminaMax = {};
    };

    struct Equipment : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        int32_t type = {};
        int32_t id = {};
        int32_t attack = {};
        int32_t defence = {};
        int32_t str = {};
        int32_t dex = {};
        int32_t intell = {};
    };

    struct CharacterEquipment : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        Equipment armor = {};
        Equipment gloves = {};
        Equipment shoes = {};
        Equipment weapon = {};
    };

    struct CharacterInventoryItem : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        int32_t id = {};
        int32_t count = {};
        int32_t attack = {};
        int32_t defence = {};
        int32_t str = {};
        int32_t dex = {};
        int32_t intell = {};
    };

    struct Character : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        int64_t instanceId = {};
        Transform transform = {};
        CharacterStat stat = {};
        CharacterEquipment equipment = {};
        int32_t gold = {};
        std::vector<CharacterInventoryItem> items = {};
    };

    struct Monster : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

    };

}
