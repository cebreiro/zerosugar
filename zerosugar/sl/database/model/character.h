#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include "zerosugar/sl/database/model/meta.h"

namespace zerosugar::sl::db
{
    struct Character
    {
        ZEROSUGAR_DATABASE_OPTION(PK)
        int64_t id = -1;

        ZEROSUGAR_DATABASE_OPTION(FK=Account.id)
        int64_t aid = -1;
        int8_t wid = 1;
        int8_t slot = -1;

        ZEROSUGAR_DATABASE_OPTION(UNIQUE, MAX_LENGTH=30)
        std::string name;

        int32_t hair_color = 0;
        int32_t hair = 0;
        int32_t skin_color = 0;
        int32_t face = 0;

        int8_t arms = 0;
        int8_t running = 0;

        int32_t gold = 0;
        int8_t inventory_page = 0;

        int32_t zone = 0;
        int32_t stage = 0;
        float x = 0.f;
        float y = 0.f;
    };

    struct CharacterStat
    {
        ZEROSUGAR_DATABASE_OPTION(UNIQUE, FK=Character.id)
        int64_t cid = -1;
        int8_t gender = 0;
        int32_t hp = 0;
        int32_t mp = 0;
        int32_t chr_lv = 1;
        int32_t chr_exp = 0;
        int32_t str = 0;
        int32_t dex = 0;
        int32_t accr = 0;
        int32_t health = 0;
        int32_t intell = 0;
        int32_t wis = 0;
        int32_t will = 0;
        int32_t stat_point = 0;
        int8_t water = 0;
        int8_t fire = 0;
        int8_t lightning = 0;
    };

    struct CharacterJob
    {
        ZEROSUGAR_DATABASE_OPTION(UNIQUE, FK=Character.id)
        int64_t cid = -1;
        int32_t job1 = -1;
        int32_t job1_level = 1;
        int32_t job1_exp = 0;
        int32_t job1_sp = 0;
        int32_t job2 = 0;
        int32_t job2_level = 0;
        int32_t job2_exp = 0;
        int32_t job2_sp = 0;
        int32_t job3 = 0;
        int32_t job3_level = 0;
        int32_t job3_exp = 0;
        int32_t job3_sp = 0;
        int32_t job_sub = 0;
        int32_t job_sub_level = 0;
        int32_t job_sub_exp = 0;
        int32_t job_sub_sp = 0;
    };

    struct CharacterItem
    {
        ZEROSUGAR_DATABASE_OPTION(PK)
        int64_t id = -1;

        ZEROSUGAR_DATABASE_OPTION(FK=Character.id)
        int64_t cid = -1;

        int32_t item_id = 0;
        int32_t quantity = 0;
        int8_t equip_position = 0;
        int8_t page = 0;
        int8_t x = 0;
        int8_t y = 0;
        int8_t is_in_quick = 0;
        int8_t is_pick = 0;
    };

    struct CharacterSkill
    {
        ZEROSUGAR_DATABASE_OPTION(FK=Character.id)
        int64_t cid = -1;
        int32_t skill_id = 0;
        int32_t job = 0;
        int32_t level = 0;
        int32_t cooldown = 0;
        int8_t page = -1;
        int8_t x = -1;
        int8_t y = -1;
        int8_t is_in_quick = 0;
    };
}
