#include "lobby_characters_get_all.h"

namespace zerosugar::xr::db::sp
{
    LobbyCharactersGetAll::LobbyCharactersGetAll(zerosugar::db::ConnectionPool::Borrowed& conn, int64_t accountId)
        : StoredProcedure(conn)
        , _accountId(accountId)
    {
    }

    auto LobbyCharactersGetAll::GetSQL() const -> std::string_view
    {
        return "CALL lobby_characters_get_all(?)";
    }

    auto LobbyCharactersGetAll::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_accountId);

        return inputs;
    }

    void LobbyCharactersGetAll::SetOutput(const boost::mysql::results& result)
    {
        const size_t size = result.size();
        if (size < 2)
        {
            return;
        }

        const boost::mysql::resultset_view& characterSet = result.at(0);
        if (characterSet.rows().empty())
        {
            return;
        }

        std::unordered_map<int64_t, service::DTOLobbyCharacter> sets;
        sets.reserve(result.size());

        for (const boost::mysql::row_view& row : characterSet.rows())
        {
            size_t index = 0;

            const int64_t characterId = row.at(index++).as_int64();
            service::DTOLobbyCharacter& character = sets[characterId];

            character.characterId = characterId;
            character.slot = static_cast<int32_t>(row.at(index++).as_int64());
            character.name = row.at(index++).as_string();
            character.level = static_cast<int32_t>(row.at(index++).as_int64());
            character.str = static_cast<int32_t>(row.at(index++).as_int64());
            character.dex = static_cast<int32_t>(row.at(index++).as_int64());
            character.intell = static_cast<int32_t>(row.at(index++).as_int64());
            character.job = static_cast<int32_t>(row.at(index++).as_int64());
            character.faceId = static_cast<int32_t>(row.at(index++).as_int64());
            character.hairId = static_cast<int32_t>(row.at(index++).as_int64());
            character.zoneId = static_cast<int32_t>(row.at(index++).as_int64());
        }

        const boost::mysql::resultset_view& itemSet = result.at(1);

        for (const boost::mysql::row_view& row : itemSet.rows())
        {
            size_t index = 0;

            const int64_t cid = row.at(index++).as_int64();
            const int32_t itemDataId = static_cast<int32_t>(row.at(index++).as_int64());
            const int32_t equipPosition = static_cast<int32_t>(row.at(index++).as_int64());

            auto iter = sets.find(cid);
            if (iter != sets.end())
            {
                service::DTOLobbyItem item;
                item.itemDataId = itemDataId;
                item.equipPosition = equipPosition;

                iter->second.items.emplace_back(item);
            }
            else
            {
                assert(false);
            }
        }
    }
}
