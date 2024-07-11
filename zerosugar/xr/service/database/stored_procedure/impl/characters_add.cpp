#include "characters_add.h"

namespace zerosugar::xr::db::sp
{
    CharactersAdd::CharactersAdd(zerosugar::db::ConnectionPool::Borrowed& conn, const service::DTOCharacterAdd& input)
        : StoredProcedure(conn)
        , _input(input)
    {
    }

    auto CharactersAdd::GetAddedCharacterId() const -> int64_t
    {
        return _resultCharacterId;
    }

    auto CharactersAdd::GetSQL() const -> std::string_view
    {
        return "CALL characters_add(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    auto CharactersAdd::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> result;
        result.emplace_back(_input.accountId);
        result.emplace_back(_input.slot);
        result.emplace_back(_input.name);
        result.emplace_back(_input.level);
        result.emplace_back(_input.str);
        result.emplace_back(_input.dex);
        result.emplace_back(_input.intell);
        result.emplace_back(_input.job);
        result.emplace_back(_input.faceId);
        result.emplace_back(_input.hairId);
        result.emplace_back(_input.gold);
        result.emplace_back(_input.zoneId);
        result.emplace_back(nullptr);

        return result;
    }

    void CharactersAdd::SetOutput(const boost::mysql::results& result)
    {
        _resultCharacterId = result.out_params().at(0).as_int64();
    }
}
