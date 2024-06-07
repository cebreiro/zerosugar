#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include "zerosugar/sl/database/model/meta.h"

namespace zerosugar::sl::db
{
    struct Item
    {
        ZEROSUGAR_DATABASE_OPTION(PK)
        int64_t id = -1;

        ZEROSUGAR_DATABASE_OPTION(INDEX)
        int64_t owner_id = -1;

        int32_t data_id = 0;
        int32_t quantity = 0;

        int8_t pos_type = 0;
        int8_t pos_value1 = 0;
        int8_t pos_value2 = 0;
        int8_t pos_value3 = 0;
    };
}
