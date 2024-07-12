#include "game_data_constant.h"

namespace zerosugar::xr
{
    DataConstant::DataConstant()
    {
        _playerAttacks = {

            PlayerAttack{
                .duration = 1.0,
                .attackEffectDelay = { 0.5 },
            },
            PlayerAttack{
                .duration = 1.163,
                .attackEffectDelay = { 0.6 },
            },
            PlayerAttack{
                .duration = 0.91,
                .attackEffectDelay = { 0.5 },
            },
            PlayerAttack{
                .duration = 2.0,
                .attackEffectDelay = { 0.6, 0.95 },
            },
        };
    }

    auto DataConstant::GetPlayerAttacks() const -> const std::array<PlayerAttack, 4>&
    {
        return _playerAttacks;
    }

    auto DataConstant::GetInstance() -> const DataConstant&
    {
        static const DataConstant instance;

        return instance;
    }
}
