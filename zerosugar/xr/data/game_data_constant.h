#pragma once
#include "zerosugar/shared/collision/shape/circle.h"
#include "zerosugar/shared/collision/shape/obb.h"

namespace zerosugar::xr
{
    struct PlayerAttack
    {
        using attack_range_type = std::variant<Eigen::AlignedBox3d, /*circle radius*/double>;

        double duration = 0.0;
        std::vector<double> attackEffectDelay;
        attack_range_type attackRange;
    };

    class DataConstant
    {
        DataConstant();

    public:
        auto GetPlayerAttacks() const -> const std::array<PlayerAttack, 4>&;

        static auto GetInstance() -> const DataConstant&;

    private:
        std::array<PlayerAttack, 4> _playerAttacks;
    };
}
