#pragma once
#include <cstdint>
#include <string>
#include "zerosugar/sl/gamedata/sox/sox.h"

namespace zerosugar::sl
{
    enum class SoxNameNotation
    {
        AllUpperUnderbar,
        AllLowerUnderbar,
        Pascal,
        Hungarian,
        ParsingFailed
    };

    class SoxCodeGenerator
    {
    public:
        SoxCodeGenerator() = delete;
        explicit SoxCodeGenerator(gamedata::Sox& sox);

        auto Generate() const -> std::pair<std::string, std::string>;
        auto GetFileName() const -> std::string;

    private:
        auto GenerateHeader() const -> std::string;
        auto GenerateCxx() const -> std::string;

    private:
        gamedata::Sox& _sox;
        SoxNameNotation _fileNotation;
        SoxNameNotation _columnNotation;
        std::string _className;
        std::string _containerName;
        int64_t _index = -1;
    };
}
