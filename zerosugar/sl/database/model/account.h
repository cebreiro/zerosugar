#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include "zerosugar/sl/database/model/meta.h"

namespace zerosugar::sl::db
{
    struct Account
    {
        ZEROSUGAR_DATABASE_OPTION(PK)
        int64_t id = -1;

        ZEROSUGAR_DATABASE_OPTION(UNIQUE, MAX_LENGTH = 30)
        std::string account;

        ZEROSUGAR_DATABASE_OPTION(MAX_LENGTH = 30)
        std::string password;

        int8_t gm_level = 0;

        std::optional<int8_t> banned = 0;

        ZEROSUGAR_DATABASE_OPTION(MAX_LENGTH = 1024)
        std::optional<std::string> ban_reason;
    };
}
