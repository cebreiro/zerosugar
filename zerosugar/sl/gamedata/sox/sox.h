#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>
#include "zerosugar/sl/gamedata/sox/sox_meta.h"

namespace zerosugar::sl::gamedata
{
    struct Sox
    {
        Sox() = default;
        explicit Sox(const std::filesystem::path& filePath);

        std::string name;
        int32_t type = -1;
        std::vector<meta::sox::Column> columnMetas;
        int32_t rowCount = -1;
        std::vector<char> data;
    };
}
