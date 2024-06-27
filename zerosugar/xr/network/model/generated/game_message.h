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

    struct PlayerBase : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        float hp = {};
        float maxHP = {};
        float attackMin = {};
        float attackMax = {};
        float attackRange = {};
        float attackSpeed = {};
        float speed = {};
        float defence = {};
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

        int32_t id = {};
        int32_t type = {};
        int32_t count = {};
        int32_t attack = {};
        int32_t defence = {};
        int32_t str = {};
        int32_t dex = {};
        int32_t intell = {};
    };

    struct PlayerEquipment : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        Equipment armor = {};
        Equipment gloves = {};
        Equipment shoes = {};
        Equipment weapon = {};
    };

    struct PlayerInventoryItem : IBufferDeserializable, IBufferSerializable
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

    struct RemotePlayer : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        int64_t id = {};
        Transform transform = {};
        PlayerBase base = {};
        PlayerEquipment equipment = {};
    };

    struct Player : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        int64_t id = {};
        Transform transform = {};
        PlayerBase base = {};
        PlayerEquipment equipment = {};
        int32_t gold = {};
        int32_t itemsCount = {};
        std::vector<PlayerInventoryItem> items = {};
    };

    struct Monster : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

    };

}
