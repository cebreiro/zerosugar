#include "equip_items_add.h"

#include "zerosugar/xr/service/model/generated/data_transfer_object_message_json.h"

namespace zerosugar::xr::db::sp
{
    EquipItemsAdd::EquipItemsAdd(zerosugar::db::ConnectionPool::Borrowed& conn, int64_t characterId,
        const std::vector<service::DTOEquipItem>& equips)
        : StoredProcedure(conn)
        , _cid(characterId)
        , _json([&equips]()
            {
                nlohmann::json result;
                to_json(result, equips);

                return result.dump();
            }())
    {
    }

    auto EquipItemsAdd::GetSQL() const -> std::string_view
    {
        return "CALL equip_items_add(?, ?)";
    }

    auto EquipItemsAdd::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> result;
        result.emplace_back(_cid);
        result.emplace_back(_json);

        return result;
    }

    void EquipItemsAdd::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}
