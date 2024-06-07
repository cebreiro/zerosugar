#include <algorithm>
#include <cstdint>
#include <span>
#include <iostream>
#include <format>
#include <ranges>
#include "zerosugar/sl/gamedata/sox/sox.h"

using namespace zerosugar;
using namespace zerosugar::sl;

void ArrangeInvalidData(gamedata::Sox& sox)
{
    for (int64_t i = 0; i < std::ssize(sox.columnMetas); ++i)
    {
        std::string& columnName = sox.columnMetas[i].name;
        if (std::ranges::any_of(columnName, [](char ch) { return ch == '*'; }))
        {
            std::string temp = columnName;
            std::ranges::transform(columnName, columnName.begin(), [](char ch)
                {
                    if (ch == '*')
                    {
                        return 'x';
                    }

                    return ch;
                });

            std::cout << std::format("invalid column: [{}] is revised to: [{}]\n", temp, columnName);
        }

        if (std::ranges::any_of(columnName, [](char ch) { return std::isspace(ch); }))
        {
            std::string temp = columnName;
            std::ranges::transform(columnName, columnName.begin(), [](char ch)
                {
                    if (std::isspace(ch))
                    {
                        return '_';
                    }

                    return ch;
                });

            std::cout << std::format("invalid column: [{}] is revised to: [{}]\n", temp, columnName);
        }

        if (columnName == "_ATTACK4RESERVED4")
        {
            if (i < std::ssize(sox.columnMetas) - 1)
            {
                std::string& next = sox.columnMetas[i + 1].name;
                if (next == "_ATTACK4RESERVED4")
                {
                    std::pair temp = { columnName , next };
                    columnName = "_ATTACK4RESERVED2";
                    next = "_ATTACK4RESERVED3";

                    std::cout << std::format("invalid column [{}, {}] is revised to: [{}, {}]\n",
                        temp.first, temp.second, columnName, next);
                }
            }
        }

        if (columnName == "_ATTACK3RESERVED3")
        {
            if (i < std::ssize(sox.columnMetas) - 1)
            {
                std::string& next = sox.columnMetas[i + 1].name;
                if (!next.compare("_ATTACK3RESERVED3"))
                {
                    std::string temp = columnName;

                    std::string& prev = sox.columnMetas[i - 1].name;
                    if (prev == "_ATTACK3RESERVED1")
                    {
                        columnName = "_ATTACK3RESERVED2";
                    }
                    else
                    {
                        columnName = "_ATTACK4RESERVED2";
                    }

                    std::cout << std::format("invalid column: [{}] is revised to: [{}]\n", temp, columnName);
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    const std::span<char*> args(argv, argc);
    if (std::ssize(args) < 3)
    {
        std::cout << "usage: this.exe <sox_input_directory> <generated_code_out_dir>\n";

        ::system("pause");
        return 1;
    }

    using namespace std::filesystem;

    const path inputPath(args[1]);
    const path outputPath(args[2]);

    try
    {
        std::vector<gamedata::Sox> soxes = directory_iterator(inputPath)
            | std::views::transform([](const directory_entry& entry) { return entry.path();  })
            | std::views::filter([](const path& path)
                {
                    if (path.has_filename() && path.has_extension())
                    {
                        return path.filename().extension().string() == ".sox";
                    }

                    return false;
                })
            | std::views::transform([](const path& path) -> gamedata::Sox
                {
                    return gamedata::Sox(path);
                })
            | std::ranges::to<std::vector>();



    }
    catch (const std::exception& e)
    {
        std::cout << std::format("fail to generate code. exception: {}\n", e.what());

        ::system("pause");
        return 1;
    }

    return 0;
}
