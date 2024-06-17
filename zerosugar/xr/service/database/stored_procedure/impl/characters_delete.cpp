#include "characters_delete.h"

namespace zerosugar::xr::db::sp
{
    CharactersDelete::CharactersDelete(zerosugar::db::ConnectionPool::Borrowed& conn, int64_t characterId)
        : StoredProcedure(conn)
        , _characterId(characterId)
    {
    }

    auto CharactersDelete::GetSQL() const -> std::string_view
    {
        return "CALL characters_delete(?)";
    }

    auto CharactersDelete::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> result;
        result.emplace_back(_characterId);

        return result;
    }

    void CharactersDelete::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}
