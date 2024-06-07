#include "sox_code_generator.h"

#include <cassert>
#include <atomic>
#include <algorithm>
#include <format>
#include <iostream>
#include <vector>
#include <ranges>

#include "zerosugar/shared/code_gen/printer.h"

namespace
{
    using namespace zerosugar::sl;

    std::vector<char> SoxHungarianSingleLiteral = { 'n', 'f', 'b', 's' };

    auto ParseFileName(const std::string& fileName) -> SoxNameNotation
    {
        if (std::ranges::all_of(fileName, [](char ch) { return std::isupper(ch) || std::isdigit(ch) || ch == '_'; }))
        {
            return SoxNameNotation::AllUpperUnderbar;
        }
        else if (std::ranges::all_of(fileName, [](char ch) { return std::islower(ch) || std::isdigit(ch) || ch == '_'; }))
        {
            return SoxNameNotation::AllLowerUnderbar;
        }

        return SoxNameNotation::Pascal;
    }

    auto ToPascal(SoxNameNotation notation, const std::string& str) -> std::string
    {
        if (notation == SoxNameNotation::Pascal)
        {
            return str;
        }

        std::string result = str;

        if (notation == SoxNameNotation::AllUpperUnderbar)
        {
            std::ranges::transform(result, result.begin(), [](char ch)
                {
                    return static_cast<char>(std::tolower(ch));
                });
        }

        result[0] = static_cast<char>(std::toupper(result[0]));
        for (int64_t i = 1; i < std::ssize(result); ++i)
        {
            if (result[i - 1] == '_')
            {
                result[i] = static_cast<char>(std::toupper(result[i]));
            }
        }

        std::erase_if(result, [](char ch)
            {
                return ch == '_';
            });

        return result;
    }

    auto ToFileNameConvention(SoxNameNotation notation, const std::string& str) -> std::string
    {
        if (notation == SoxNameNotation::Pascal)
        {
            std::string ret;

            for (size_t i = 0; i < str.length(); ++i)
            {
                char ch = str[i];
                if (std::isupper(ch))
                {
                    if (i != 0)
                    {
                        ret += '_';
                    }

                    ret += static_cast<char>(std::tolower(ch));
                }
                else
                {
                    ret += ch;
                }
            }

            return ret;
        }

        std::string ret;
        std::ranges::transform(str, std::back_inserter(ret), [](char ch)
            {
                return static_cast<char>(std::tolower(ch));
            });

        return ret;
    }

    bool CheckHungarianTypeString(const std::string& str)
    {
        if (std::ranges::any_of(SoxHungarianSingleLiteral, [&str](char ch) { return ch == str[0]; }))
        {
            if (std::isupper(str[1]))
            {
                return true;
            }
        }

        if (str.starts_with("sz") && std::isupper(str[2]))
        {
            return true;
        }

        return false;
    }

    auto ParseColumnName(const std::string& columnName) -> SoxNameNotation
    {
        if (std::ranges::all_of(columnName, [](char ch) { return std::isupper(ch) || std::isdigit(ch) || ch == '_'; }))
        {
            return SoxNameNotation::AllUpperUnderbar;
        }
        else if (std::ranges::all_of(columnName, [](char ch) { return std::islower(ch) || std::isdigit(ch) || ch == '_'; }))
        {
            return SoxNameNotation::AllLowerUnderbar;
        }
        else if (CheckHungarianTypeString(columnName))
        {
            return SoxNameNotation::Hungarian;
        }

        std::cout << std::format("sox column notation parse fail, column name: [{}]\n", columnName);

        return SoxNameNotation::ParsingFailed;
    }

    auto ToMemberConvention(SoxNameNotation notation, const std::string& str) -> std::string
    {
        // exceptional case handling
        if (!str.compare("_index"))
        {
            return "index";
        }
        else if (!str.compare("_szName"))
        {
            return "name";
        }

        std::string result = str;

        if (notation == SoxNameNotation::Hungarian)
        {
            if (str.starts_with("sz"))
            {
                result.erase(result.begin(), result.begin() + 2);
            }
            else if (std::ranges::any_of(SoxHungarianSingleLiteral, [&str](char ch) { return ch == str[0]; }))
            {
                result.erase(result.begin());
            }
            else
            {
                assert(false);
            }
        }
        else if (notation == SoxNameNotation::AllUpperUnderbar)
        {
            std::ranges::transform(result, result.begin(), [](char ch)
                {
                    return static_cast<char>(std::tolower(ch));
                });
        }

        for (int64_t i = 1; i < std::ssize(result); ++i)
        {
            if (result[i - 1] == '_')
            {
                result[i] = static_cast<char>(std::toupper(result[i]));
            }
        }

        std::erase_if(result, [](char ch)
            {
                return ch == '_';
            });

        result[0] = static_cast<char>(std::tolower(result[0]));

        if (std::isdigit(result[0]) || result =="int" || result == ("default"))
        {
            result.insert(0, "_");
        }

        return result;
    }
}

namespace zerosugar::sl
{
    static std::atomic<int64_t> index = 1;

    SoxCodeGenerator::SoxCodeGenerator(gamedata::Sox& sox)
        : _sox(sox)
        , _fileNotation(ParseFileName(_sox.name))
        , _columnNotation(ParseColumnName(_sox.columnMetas.at(1).name))
        , _className(ToPascal(_fileNotation, _sox.name))
        , _containerName(std::format("{}Table", _className))
        , _index(index.fetch_add(1))
    {
        // TODO
        // 1. time_service 만들기->current since epoch 반환
        // 2. time_service 가 snowflake 발급해주기
    }

    auto SoxCodeGenerator::Generate() const -> std::pair<std::string, std::string>
    {
    }

    auto SoxCodeGenerator::GetFileName() const -> std::string
    {
        return ToFileNameConvention(_fileNotation, _sox.name);
    }

    auto SoxCodeGenerator::GenerateHeader() const -> std::string
    {
    }

    auto SoxCodeGenerator::GenerateCxx() const -> std::string
    {
    }
}
