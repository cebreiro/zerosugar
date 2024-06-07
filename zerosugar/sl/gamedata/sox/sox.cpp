#include "sox.h"

namespace zerosugar::sl::gamedata
{
    auto ReadFile(const std::filesystem::path& filePath) -> std::vector<char>
    {
        std::ifstream ifs(filePath, std::ios::binary);
        if (!ifs.is_open())
        {
            throw std::runtime_error(std::format("file open fail. {}", filePath.string()));
        }

        const int64_t fileSize = [](std::ifstream& ifs)
            {
                const int64_t current = ifs.tellg();

                ifs.seekg(0, std::ios::end);
                const int64_t size = ifs.tellg();

                ifs.seekg(current, std::ios::beg);

                return size;
            }(ifs);

        std::vector<char> buffer(fileSize);

        ifs.read(buffer.data(), fileSize);
        ifs.close();

        return buffer;
    }

    Sox::Sox(const std::filesystem::path& filePath)
    {
        const std::vector<char>& buffer = ReadFile(filePath);;

        const char* fileData = buffer.data();
        const int64_t fileSize = std::ssize(buffer);

        StreamReader reader(buffer.begin(), buffer.end());

        name = filePath.stem().string();
        type = reader.Read<int32_t>();
        const int32_t columnCount = reader.Read<int32_t>();
        rowCount = reader.Read<int32_t>();

        // exception handling '_index' column
        columnMetas.resize(columnCount + 1);
        columnMetas[0] = meta::sox::Column{ meta::sox::ValueType::Int, "_index" };

        for (int32_t i = 0; i < columnCount; ++i)
        {
            constexpr int64_t max_name_buffer_size = 64;
            std::array<char, max_name_buffer_size> names = {};

            reader.ReadBuffer(names.data(), max_name_buffer_size);

            columnMetas[i].name = std::string(names.data());
        }

        constexpr int64_t type_section_offset = 0x200C;
        reader.SetOffset(type_section_offset);

        for (int32_t i = 0; i < columnCount; ++i)
        {
            columnMetas[i + 1].valueType = meta::sox::ToValueType(reader.Read<int32_t>());
        }

        constexpr int64_t data_section_offset = 0x220C;
        reader.SetOffset(data_section_offset);

        data.reserve(fileSize - data_section_offset);
        std::ranges::copy(fileData + type_section_offset, fileData + fileSize, std::back_inserter(data));

        for (int32_t i = 0; i < rowCount; ++i)
        {
            for (const meta::sox::Column& column : columnMetas)
            {
                switch (column.valueType)
                {
                case meta::sox::ValueType::String:
                    reader.Skip(reader.Read<int16_t>());
                    break;
                case meta::sox::ValueType::Int:
                case meta::sox::ValueType::Float:
                    reader.Skip(4);
                    break;
                case meta::sox::ValueType::None:
                default:
                    throw std::runtime_error("unknown sox type");
                }
            }
        }

        const std::string& string = reader.ReadString(5);
        if (string != "THEND")
        {
            throw std::runtime_error("fail to find `THEND`. invalid sox format");
        }
    }
}
