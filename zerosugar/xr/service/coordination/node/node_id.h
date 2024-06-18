#pragma once

namespace zerosugar::xr::coordination
{
    using game_server_id_type = ValueType<int64_t, class GameServerIdTag>;
    using game_instance_id_type = ValueType<int64_t, class GameInstanceIdTag>;
    using game_user_id_type = ValueType<int64_t, class GameUserIdTag>;
}
