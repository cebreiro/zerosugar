#include "characters_get.h"

namespace zerosugar::xr::db::sp
{
    CharactersGet::CharactersGet(zerosugar::db::ConnectionPool::Borrowed& conn, int64_t characterId)
        : StoredProcedure(conn)
        , _characterId(characterId)
    {
    }

    auto CharactersGet::GetResult() const -> const service::DTOCharacter&
    {
        return _result;
    }

    auto CharactersGet::GetSQL() const -> std::string_view
    {
        return "CALL characters_get(?)";
    }

    auto CharactersGet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> result;
        result.emplace_back(_characterId);

        return result;
    }

    void CharactersGet::SetOutput(const boost::mysql::results& result)
    {
        const size_t size = result.size();
        if (size < 3)
        {
            throw std::runtime_error(std::format("invalid result"));
        }

        const boost::mysql::resultset_view& characterSet = result.at(0);
        if (characterSet.rows().empty())
        {
            throw std::runtime_error(std::format("invalid result. fail to find character"));
        }
        else
        {
            const boost::mysql::row_view& row = characterSet.rows().front();

            size_t index = 0;

            _result.characterId = row.at(index++).as_int64();
            _result.name = row.at(index++).as_string();
            _result.level = _result.str = static_cast<int32_t>(row.at(index++).as_int64());
            _result.str = static_cast<int32_t>(row.at(index++).as_int64());
            _result.dex = static_cast<int32_t>(row.at(index++).as_int64());
            _result.intell = static_cast<int32_t>(row.at(index++).as_int64());
            _result.job = static_cast<int32_t>(row.at(index++).as_int64());
            _result.faceId = static_cast<int32_t>(row.at(index++).as_int64());
            _result.hairId = static_cast<int32_t>(row.at(index++).as_int64());
            _result.zoneId = static_cast<int32_t>(row.at(index++).as_int64());
        }

        const boost::mysql::resultset_view& itemSet = result.at(1);
        {
            _result.items.reserve(itemSet.rows().size());

            constexpr auto makeOptional = []<typename T>(const boost::mysql::field_view & field, [[maybe_unused]] T tag)
            {
                if constexpr (std::is_integral_v<T>)
                {
                    if (field.is_null())
                    {
                        return std::optional<T>();
                    }
                    else
                    {
                        return std::optional<T>(static_cast<T>(field.as_int64()));
                    }
                }
                else
                {
                    static_assert(sizeof(T), "not implemented");
                }
            };

            for (const boost::mysql::row_view& row : itemSet.rows())
            {
                service::DTOItem& item = _result.items.emplace_back();

                size_t index = 0;

                item.itemId = row.at(index++).as_int64();
                item.itemDataId = static_cast<int32_t>(row.at(index++).as_int64());
                item.quantity = static_cast<int32_t>(row.at(index++).as_int64());
                item.slot = makeOptional(row.at(index++), int8_t{});

                item.attack = makeOptional(row.at(index++), int32_t{});
                item.defence = makeOptional(row.at(index++), int32_t{});
                item.str = makeOptional(row.at(index++), int32_t{});
                item.dex = makeOptional(row.at(index++), int32_t{});
                item.intell = makeOptional(row.at(index++), int32_t{});
            }
        }

        const boost::mysql::resultset_view& equipItemSet = result.at(2);
        {
            _result.equipments.reserve(equipItemSet.rows().size());

            for (const boost::mysql::row_view& row : equipItemSet.rows())
            {
                service::DTOEquipment& equipment = _result.equipments.emplace_back();

                size_t index = 0;

                equipment.itemId = row.at(index++).as_int64();
                equipment.equipPosition = static_cast<int32_t>(row.at(index++).as_int64());
            }
        }
    }
}
