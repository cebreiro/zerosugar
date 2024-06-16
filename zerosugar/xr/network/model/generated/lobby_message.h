#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "zerosugar/shared/network/buffer/buffer_deserializable.h"
#include "zerosugar/shared/network/buffer/buffer_serializable.h"

namespace zerosugar::xr::network::lobby
{
    struct LobbyCharacter : IBufferDeserializable, IBufferSerializable
    {
        void Deserialize(BufferReader& reader) final;
        void Serialize(BufferWriter& writer) const final;

        int32_t slot = {};
        std::string name = {};
        int32_t level = {};
        int32_t str = {};
        int32_t dex = {};
        int32_t intell = {};
        int32_t job = {};
        int32_t faceId = {};
        int32_t hairId = {};
        int32_t gold = {};
        int32_t zoneId = {};
        float x = {};
        float y = {};
        float z = {};
        int32_t armorId = {};
        int32_t glovesId = {};
        int32_t shoesId = {};
        int32_t weaponId = {};
        int32_t gender = {};
    };

}
