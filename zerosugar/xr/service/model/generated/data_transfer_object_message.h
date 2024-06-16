#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

namespace zerosugar::xr::service
{
    struct DTOAccount
    {
        int64_t accountId = {};
        std::string account = {};
        std::string password = {};
        int32_t gmLevel = {};
        int32_t banned = {};
        int32_t deleted = {};
    };

    struct DTOItemOption
    {
        int32_t attack = {};
        int32_t defence = {};
        int32_t str = {};
        int32_t dex = {};
        int32_t intell = {};
    };

    struct DTOItem
    {
        int64_t itemId = {};
        int32_t itemDataId = {};
        int32_t quantity = {};
        std::optional<DTOItemOption> option = {};
    };

    struct DTOEquipItem
    {
        DTOItem item = {};
        int32_t equipPosition = {};
    };

    struct DTOCharacterAdd
    {
        int64_t accountId = {};
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
    };

    struct DTOLobbyItem
    {
        int32_t itemDataId = {};
        int32_t equipPosition = {};
    };

    struct DTOLobbyCharacter
    {
        int64_t characterId = {};
        int32_t slot = {};
        std::string name = {};
        int32_t level = {};
        int32_t str = {};
        int32_t dex = {};
        int32_t intell = {};
        int32_t job = {};
        int32_t faceId = {};
        int32_t hairId = {};
        int32_t zoneId = {};
        std::vector<DTOLobbyItem> items = {};
    };

}
