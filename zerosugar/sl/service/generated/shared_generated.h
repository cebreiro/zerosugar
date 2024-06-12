#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

namespace zerosugar::sl::service
{
    enum class SlotPositionType : int64_t
    {
        SlotPositionNone = 0,
        SlotPositionInventory = 1,
        SlotPositionQuickSlot = 2,
        SlotPositionEquipment = 3,
        SlotPositionPicked = 4,
    };
    enum class JobGradeType : int64_t
    {
        JobGradeNovice = 0,
        JobGradeAdvanced1 = 1,
        JobGradeAdvanced2 = 2,
        JobGradeSub = 3,
        JobGradeMax = 4,
    };
    struct AuthToken
    {
        std::string values = {};
    };

    struct Account
    {
        int64_t id = {};
        std::string account = {};
        std::string password = {};
        int64_t gmLevel = {};
        bool banned = {};
        std::string banReason = {};
    };

    struct AccountUpdate
    {
        int64_t id = {};
        std::optional<std::string> password = {};
        std::optional<int64_t> gmLevel = {};
        std::optional<bool> banned = {};
        std::optional<std::string> banReason = {};
    };

    struct SlotPosition
    {
        SlotPositionType type = {};
        int32_t value1 = {};
        int32_t value2 = {};
        int32_t value3 = {};
    };

    struct Item
    {
        int64_t uniqueId = {};
        int64_t ownerId = {};
        int32_t dataId = {};
        int32_t quantity = {};
        SlotPosition position = {};
    };

    struct Skill
    {
        int64_t uniqueId = {};
        int64_t ownerId = {};
        int32_t jobId = {};
        int32_t skillId = {};
        int32_t level = {};
        int64_t cooldown = {};
        SlotPosition position = {};
    };

    struct Job
    {
        JobGradeType grade = {};
        int32_t dataId = {};
        int32_t level = {};
        int32_t exp = {};
        int32_t sp = {};
    };

    struct CharacterStat
    {
        int32_t gender = {};
        int32_t hp = {};
        int32_t mp = {};
        int32_t level = {};
        int32_t exp = {};
        int32_t str = {};
        int32_t dex = {};
        int32_t accr = {};
        int32_t health = {};
        int32_t intell = {};
        int32_t wis = {};
        int32_t will = {};
        int32_t statPoint = {};
        int32_t elementalWater = {};
        int32_t elementalFire = {};
        int32_t elementalLightning = {};
    };

    struct Character
    {
        int64_t id = {};
        int32_t slot = {};
        std::string name = {};
        int32_t hairColor = {};
        int32_t hair = {};
        int32_t skinColor = {};
        int32_t face = {};
        bool arms = {};
        bool running = {};
        int32_t gold = {};
        int32_t inventorySize = {};
        int32_t zone = {};
        int32_t stage = {};
        float x = {};
        float y = {};
        CharacterStat stat = {};
        std::vector<Job> job = {};
        std::vector<Item> item = {};
        std::vector<Skill> skill = {};
    };

}
