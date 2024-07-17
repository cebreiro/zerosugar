#include "game_data_constant.h"

namespace zerosugar::xr
{
    DataConstant::DataConstant()
    {
        _playerAttacks = {
            PlayerAttack{
                .duration = 1.0,
                .attackEffectDelay = { 0.5 },
                .attackRange = Eigen::AlignedBox3d(
                    Eigen::Vector3d(0, -120, 0),
                    Eigen::Vector3d(600.f, 120.f, 100.f)),
            },
            PlayerAttack{
                .duration = 1.163,
                .attackEffectDelay = { 0.6 },
                .attackRange = Eigen::AlignedBox3d(
                    Eigen::Vector3d(0, -120, 0),
                    Eigen::Vector3d(600.f, 120.f, 100.f)),
            },
            PlayerAttack{
                .duration = 0.91,
                .attackEffectDelay = { 0.5 },
                .attackRange = Eigen::AlignedBox3d(
                    Eigen::Vector3d(0, -200, 0),
                    Eigen::Vector3d(600.f, 200.f, 100.f)),
            },
            PlayerAttack{
                .duration = 2.0,
                .attackEffectDelay = { 0.6, 0.95 },
                .attackRange = 400.0,
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
