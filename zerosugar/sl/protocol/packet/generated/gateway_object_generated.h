#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "zerosugar/shared/network/buffer/buffer_deserializable.h"
#include "zerosugar/shared/network/buffer/buffer_serializable.h"

namespace zerosugar::sl::gateway
{
    struct UnknownObject : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

    };

    struct EndPoint : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        int32_t port = {};
        std::string address = {};
    };

    struct CharacterAppearance : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        int32_t hatModelId = {};
        int32_t hairColor = {};
        int32_t skinColor = {};
        int32_t face = {};
        int32_t unk01 = {};
        int32_t unk02 = {};
        int32_t hairModelId = {};
        int32_t unk03 = {};
        int32_t unk04 = {};
        int32_t jacketItemId = {};
        int32_t unk05 = {};
        int32_t unk06 = {};
        int32_t glovesModelId = {};
        int32_t unk07 = {};
        int32_t unk08 = {};
        int32_t pantsId = {};
        int32_t unk09 = {};
        int32_t unk10 = {};
        int32_t shoesId = {};
        int32_t unk11 = {};
        int32_t unk12 = {};
        int32_t unk13 = {};
        int32_t unk14 = {};
        int32_t unk15 = {};
    };

    struct CharacterInformation : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        std::array<char, 20> name = {};
        std::array<char, 12> unk01 = {};
        std::array<char, 32> unk02 = {};
        std::array<char, 4> zone = {};
        std::array<char, 28> unk03 = {};
        std::array<char, 32> unk04 = {};
        std::array<char, 28> unk05 = {};
        std::array<char, 4> job = {};
        bool arms = {};
        int32_t unk06 = {};
        int8_t slot = {};
        int32_t unk07 = {};
        bool appearanceIfTrueItemIdElseModelId = {};
        std::array<char, 5> unk08 = {};
        int32_t job1 = {};
        int32_t job2 = {};
        std::array<char, 20> unk09 = {};
        std::array<char, 20> unk10 = {};
        std::array<char, 24> unk11 = {};
        int32_t gender = {};
        int32_t characterLevel = {};
    };

    struct Character : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        CharacterAppearance appearance = {};
        CharacterInformation information = {};
    };

    struct CharacterCreate : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        std::array<char, 32> name = {};
        int32_t gender = {};
        int32_t str = {};
        int32_t dex = {};
        int32_t accr = {};
        int32_t health = {};
        int32_t intell = {};
        int32_t wis = {};
        int32_t will = {};
        int32_t elementalWater = {};
        int32_t elementalFire = {};
        int32_t elementalLightning = {};
        int32_t unk01 = {};
        int32_t hairColor = {};
        int32_t skinColor = {};
        int32_t face = {};
        int32_t hair = {};
        int32_t jacketId = {};
        int32_t glovesModelId = {};
        int32_t pantsId = {};
        int32_t shoesId = {};
        int32_t unk02 = {};
        int32_t unk03 = {};
        int32_t unk04 = {};
        int32_t unk05 = {};
        int32_t unk06 = {};
        int32_t unk07 = {};
        int32_t unk08 = {};
        int32_t unk09 = {};
        int32_t unk10 = {};
        int32_t unk11 = {};
        int32_t unk12 = {};
        int32_t unk13 = {};
        int32_t unk14 = {};
        int32_t unk15 = {};
        int32_t unk16 = {};
        int32_t unk17 = {};
        int32_t job = {};
    };

}
